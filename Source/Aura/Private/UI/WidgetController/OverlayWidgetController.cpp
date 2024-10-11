// P.S. project


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
	
	OnXPPercentChangedDelegate.Broadcast(GetAuraPS()->GetXP());
	//OnPlayerLevelChanged.Broadcast(GetAuraPS()->GetPlayerLevel());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddLambda([this](int32 NewValue, bool bLevelUp)
	{
		OnPlayerLevelChanged.Broadcast(NewValue, bLevelUp);
	});
	
	/* Binding callbacks to the ASC's value changed delegates which broadcast our delegates to widgets */
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);

	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
	
	// Binding a callback for a AbilitiesGivenDelegate, broadcast when abilities are given on startup if they weren't given yet
	if (!GetAuraASC()->bStartupAbilitiesGiven)
	{
		GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
	}
	// call the callback directly if the abilities were already given
	else
	{
		BroadcastAbilityInfo();
	}
		
	/* EffectAssetTagsDelegate is broadcast in Aura ASC when any Effect is applied
	 * here we bind Lambda callback to this delegate to check all Tags gained from the applied Effect and find Message Tags
	 * then we broadcast our MessageWidgetRowDelegate where we search our Data Table (defined in BP) for the required Tag by name
	 * to show the message and other related info in WBP_EffectMessage
	 * (the callback for MessageWidgetRowDelegate is assigned and defined in WBP_Overlay)
	 */
	GetAuraASC()->EffectAssetTagsDelegate.AddLambda(
	[this](const FGameplayTagContainer& AssetTags)
		{
			for (const auto& Tag : AssetTags)
			{
				/* for example: Tag = Message.HealthPotion
				 * "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False */
        			
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);				
				}
			}
		}
	);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo, fill out PlayerState blueprint"));
	
	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].ExpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].ExpRequirement;

		const int32 DeltaLevelUpRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelUpRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}


	int32 TotalXP = NewXP;
	int32 LookUpLevel = 1;
	
	while (LookUpLevel <= Level)
	{
		TotalXP -= LevelUpInfo->LevelUpInformation[LookUpLevel].ExpRequirement;
		LookUpLevel++;
	}
	float Percent = static_cast<float>(TotalXP / LevelUpInfo->LevelUpInformation[LookUpLevel].ExpRequirement);
	UE_LOG(LogTemp, Warning, TEXT(""));
	
	
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& InputTag, const FGameplayTag& PreviousInputTag) const
{
	// clear out old spell slot
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"));
	LastSlotInfo.InputTag = PreviousInputTag;
	LastSlotInfo.AbilityTag = FGameplayTag::RequestGameplayTag(FName("Abilities.None"));
	// broadcast empty info if we're equipping an already equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	// fill in new spell slot
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = InputTag;
	AbilityInfoDelegate.Broadcast(Info);
	
}

