// P.S. project


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// bind our callbacks to the ASC delegates when InitAbilityActorInfo() is set on the Character class
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
	
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const auto& AbilityClass : StartupAbilities)
	{
		// create a Spec for a given Gameplay Ability
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		// cast Ability to UAuraGameplayAbility to get its StartupInputTag
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			// add StartupInputTag to the Spec's DynamicAbilityTags container to be able to change it at runtime
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped")));
			// grant ability to the character
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const auto& AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopedLock(*this);
	// loop through all ActivatableAbilities to find the one that has corresponding InputTag,
	// set InputPressed bool and try to activate the Ability
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// InputPressed virtual func on ASC is used to define custom logic for input. It doesn't do anything by default
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			
			// function call needed for WaitInputRelease task
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// InputPressed virtual func on ASC is used to define custom logic for input. It doesn't do anything by default
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				// function call needed for WaitInputPress task
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// FScopedAbilityListLock is used to lock the scope of the function 
	// preventing abilities from being added or removed from the given ASC when iterating over them
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			// __FUNCTION__ macro return the name of the function we're in
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag("Abilities.Status")))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusFromTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecFromAbilityTag(AbilityTag))
	{
		return GetAbilityStatusFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetAbilitySpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTagExact(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetAbilitySpecFromInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsInputTagOccupied(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilityHasInputTag(AbilitySpec, InputTag))
		{
			return true;
		}
	}
	return false;
}

bool UAuraAbilitySystemComponent::AbilityHasInputTag(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	return AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag);
}

bool UAuraAbilitySystemComponent::AbilityHasAnyInputTag(const FGameplayAbilitySpec& AbilitySpec)
{
	for (auto& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return true;
		}	
	}
	return false;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(GetAbilityTagFromSpec(AbilitySpec), true);
	
	return Info.AbilityType.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Type.Passive")));
}

void UAuraAbilitySystemComponent::AssignInputTagToAbility(FGameplayAbilitySpec& AbilitySpec,
	const FGameplayTag& InputTag)
{
	ClearInputTag(&AbilitySpec);
	AbilitySpec.DynamicAbilityTags.AddTag(InputTag);
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	ServerUpgradeAttribute(AttributeTag);
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const auto& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Level < Info.LevelRequirement) continue;
		
		// if the ability is already granted, we don't want to do anything to its status
		if (GetAbilitySpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			const FGameplayTag EligibleStatusTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible"));
			AbilitySpec.DynamicAbilityTags.AddTag(EligibleStatusTag);
			GiveAbility(AbilitySpec);
			// force replication
			MarkAbilitySpecDirty(AbilitySpec, true);
			ClientUpdateAbilityStatus(Info.AbilityTag, EligibleStatusTag, 1);
		}	
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& InputTag)
{
	// first make sure the ability exists on the ASC
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecFromAbilityTag(AbilityTag))
	{
		const FGameplayTag& PreviousInputTag = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& Status = GetAbilityStatusFromSpec(*AbilitySpec);

		const FGameplayTag& EquippedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"));
		const FGameplayTag& UnlockedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"));

		if (PreviousInputTag == InputTag) return;
		if (Status == EquippedTag || Status == UnlockedTag)
		{
			// if there's an ability occupying this InputTag already, clear its input tag and deactivate it if it's a passive ability
			if (IsInputTagOccupied(InputTag))
			{
				if (FGameplayAbilitySpec* SpecWithInputTag = GetAbilitySpecFromInputTag(InputTag))
				{
					// return early if occupied ability and the one we want to equip are the same
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithInputTag)))
					{
						ClientEquipAbility(AbilityTag, EquippedTag, InputTag, PreviousInputTag);
						return;
					}
					if (IsPassiveAbility(*SpecWithInputTag))
					{
						DeactivatePassiveAbilityDelegate.Broadcast(GetAbilityTagFromSpec(*SpecWithInputTag));
						
					}
					ClearInputTag(SpecWithInputTag);
				}
			}

			// ability doesn't have InputTag (is not active)
			if (!AbilityHasAnyInputTag(*AbilitySpec))
			{
				// activate passive ability
				if (IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
				}
			}

			// update ability status if it was unlocked and became equipped
			if (Status == UnlockedTag)
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(UnlockedTag);
				AbilitySpec->DynamicAbilityTags.AddTag(EquippedTag);
			}
			// assign new input tag to the Ability, mark it for replication and call client RPC to update its status on client
			AssignInputTagToAbility(*AbilitySpec, InputTag);
			MarkAbilitySpecDirty(*AbilitySpec);
			ClientEquipAbility(AbilityTag, EquippedTag, InputTag, PreviousInputTag);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PreviousInputTag)
{
	AbilityEquippedDelegate.Broadcast(AbilityTag, StatusTag, InputTag, PreviousInputTag);
}

bool UAuraAbilitySystemComponent::GetDescriptionsForTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                        FString& OutNextLvlDescription)
{
	// make sure the ability exists among activatable abilities on out ASC
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecFromAbilityTag(AbilityTag))
	{
		UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability);
		OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
		OutNextLvlDescription = AuraAbility->GetNextLvlDescription(AbilitySpec->Level + 1);
		return true;
	}
	// return locked ability description
	if (const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
		OutNextLvlDescription = FString();
		return false;
	}
	OutDescription = FString();
	OutNextLvlDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::ClearInputTag(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag InputTag = GetInputTagFromSpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(InputTag);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			ClearInputTag(&Spec);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecFromAbilityTag(AbilityTag))
	{
		const FGameplayTag EligibleTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible"));
		const FGameplayTag UnlockedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"));
		const FGameplayTag EquippedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"));
		const FGameplayTag LockedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Locked"));
		
		FGameplayTag AbilityStatus = GetAbilityStatusFromSpec(*AbilitySpec);
		if (AbilityStatus == LockedTag) return;

		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		
		// unlock ability
		if (AbilityStatus.MatchesTagExact(EligibleTag))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(EligibleTag);
			AbilitySpec->DynamicAbilityTags.AddTag(UnlockedTag);
			AbilityStatus = UnlockedTag;
		}
		// level up ability if it's already unlocked or equipped
		else if (AbilityStatus.MatchesTagExact(EquippedTag) || AbilityStatus.MatchesTagExact(UnlockedTag))
		{
			AbilitySpec->Level += 1;
		}
		ClientUpdateAbilityStatus(AbilityTag, AbilityStatus, AbilitySpec->Level);
		// force replication of the AbilitySpec
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	// broadcast our delegate so the WidgetController (if bound) can respond with its callback
	EffectAssetTagsDelegate.Broadcast(TagContainer);
	
}
