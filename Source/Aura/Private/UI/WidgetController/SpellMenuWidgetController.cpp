// P.S. project


#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	OnSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
		{
			if (SelectedAbility.AbilityTag == AbilityTag && SelectedAbility.StatusTag != StatusTag)
			{
				SelectedAbility.StatusTag = StatusTag;
				bool bEnableSpellBtn;
				bool bEnableEquipBtn;
				ShouldEnableButtons(SelectedAbility.StatusTag, GetAuraPS()->GetSpellPoints(), bEnableSpellBtn, bEnableEquipBtn);
				OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpellBtn, bEnableEquipBtn);
			}
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		});
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 InPoints)
		{
			if (SelectedAbility.AbilityTag.IsValid() &&
				SelectedAbility.AbilityTag != FGameplayTag::RequestGameplayTag(FName("Abilities.None")))
			{
				bool bEnableSpellBtn;
				bool bEnableEquipBtn;
				ShouldEnableButtons(SelectedAbility.StatusTag, InPoints, bEnableSpellBtn, bEnableEquipBtn);
				OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpellBtn, bEnableEquipBtn);	
			}
			OnSpellPointsChanged.Broadcast(InPoints);	
		});
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (!AbilityTag.IsValid() || !IsValid(GetAuraASC())) return;
	if (AbilityTag == FGameplayTag::RequestGameplayTag(FName("Abilities.None")))
	{
		OnSpellGlobeSelectedDelegate.Broadcast(false, false);
		return;
	}
	
	SelectedAbility.AbilityTag = AbilityTag;
	if (FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetAbilitySpecFromTag(AbilityTag))
	{
		SelectedAbility.StatusTag = GetAuraASC()->GetAbilityStatusFromSpec(*AbilitySpec);

		bool bEnableSpellBtn;
		bool bEnableEquipBtn;
		ShouldEnableButtons(SelectedAbility.StatusTag, GetAuraPS()->GetSpellPoints(), bEnableSpellBtn, bEnableEquipBtn);
		OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpellBtn, bEnableEquipBtn);
	}
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& StatusTag, int32 InSpellPoints, bool& bEnableSpendPointBtn,
	bool& bEnableEquipBtn)
{
	const FGameplayTag EligibleTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible"));
	const FGameplayTag UnlockedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"));
	const FGameplayTag EquippedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"));

	bool bSpellPointBtn = false;
	bool bEquipBtn = false;
	
	if (StatusTag == EligibleTag || StatusTag == EquippedTag)
	{
		if (InSpellPoints > 0)
		{
			bSpellPointBtn = true;
		}
	}
	else if (StatusTag == UnlockedTag)
	{
		if (InSpellPoints > 0)
		{
			bSpellPointBtn = true;
		}
		bEquipBtn = true;
	}

	bEnableSpendPointBtn = bSpellPointBtn;
	bEnableEquipBtn = bEquipBtn;
}
