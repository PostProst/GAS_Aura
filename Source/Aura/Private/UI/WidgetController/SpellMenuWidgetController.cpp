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
			if (SelectedAbility.AbilityTag == AbilityTag)
			{
				SelectedAbility.StatusTag = StatusTag;
				bool bEnableSpellBtn;
				bool bEnableEquipBtn;
				ShouldEnableButtons(SelectedAbility.StatusTag, GetAuraPS()->GetSpellPoints(), bEnableSpellBtn, bEnableEquipBtn);
				FString Description;
				FString NextLvlDescription;
				GetAuraASC()->GetDescriptionsForTag(AbilityTag, Description, NextLvlDescription);
				OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpellBtn, bEnableEquipBtn, Description, NextLvlDescription);
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
				FString Description;
				FString NextLvlDescription;
				GetAuraASC()->GetDescriptionsForTag(SelectedAbility.AbilityTag, Description, NextLvlDescription);
				OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpellBtn, bEnableEquipBtn, Description, NextLvlDescription);	
			}
			OnSpellPointsChanged.Broadcast(InPoints);	
		});

	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
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
		OnSpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
		if(bWaitingForEquipSelection)
		{
			StopWaitingForEquipDelegate.Broadcast();
			bWaitingForEquipSelection = false;
		}
		return;
	}
	SelectedAbility.AbilityTag = AbilityTag;
	if (FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetAbilitySpecFromAbilityTag(AbilityTag))
	{
		SelectedAbility.StatusTag = GetAuraASC()->GetAbilityStatusFromSpec(*AbilitySpec);
	}
	else
	{
		SelectedAbility.StatusTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Locked"));
	}
	
	if (bWaitingForEquipSelection)
	{
		StopWaitingForEquipDelegate.Broadcast();
		bWaitingForEquipSelection = false;
	}
	
	bool bEnableSpellBtn;
	bool bEnableEquipBtn;
	ShouldEnableButtons(SelectedAbility.StatusTag, GetAuraPS()->GetSpellPoints(), bEnableSpellBtn, bEnableEquipBtn);
	FString Description;
	FString NextLvlDescription;
	GetAuraASC()->GetDescriptionsForTag(SelectedAbility.AbilityTag, Description, NextLvlDescription);
	OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpellBtn, bEnableEquipBtn, Description, NextLvlDescription);
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& StatusTag, int32 InSpellPoints, bool& bEnableSpendPointBtn,
	bool& bEnableEquipBtn)
{
	const FGameplayTag EligibleTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible"));
	const FGameplayTag UnlockedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"));
	const FGameplayTag EquippedTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"));

	bool bSpellPointBtn = false;
	bool bEquipBtn = false;
	
	if (StatusTag == EligibleTag)
	{
		if (InSpellPoints > 0)
		{
			bSpellPointBtn = true;
		}
	}
	else if (StatusTag == UnlockedTag || StatusTag == EquippedTag)
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

void USpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		StopWaitingForEquipDelegate.Broadcast();
		bWaitingForEquipSelection = false;
	}
	
	SelectedAbility.AbilityTag = FGameplayTag::RequestGameplayTag(FName("Abilities.None"));
	SelectedAbility.StatusTag = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Locked"));
	OnSpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityType;
	WaitForEquipDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = true;

	// if we press equip on already equipped spell, we must cash its current input tag to clear it after reassigning
	if (SelectedAbility.StatusTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"))))
	{
		SelectedAbilityInputSlot = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).InputTag;
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& InputTag, const FGameplayTag& InGlobeAbilityType)
{
	if (!bWaitingForEquipSelection) return;
	// check whether we try to assign an offensive ability to a passive slot and vice versa
	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(InGlobeAbilityType)) return;
	if (GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag) == InputTag) return;
	
	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag, InputTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& InputTag, const FGameplayTag& PreviousInputTag)
{
	bWaitingForEquipSelection = false;

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

	StopWaitingForEquipDelegate.Broadcast();
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();
}
