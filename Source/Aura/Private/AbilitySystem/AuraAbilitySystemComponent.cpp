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
	
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
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

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetAbilitySpecFromTag(const FGameplayTag& AbilityTag)
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
		if (GetAbilitySpecFromTag(Info.AbilityTag) == nullptr)
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

bool UAuraAbilitySystemComponent::GetDescriptionsForTag(const FGameplayTag& AbilityTag, FString& OutDescription,
	FString& OutNextLvlDescription)
{
	// make sure the ability exists among activatable abilities on out ASC
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecFromTag(AbilityTag))
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

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecFromTag(AbilityTag))
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
