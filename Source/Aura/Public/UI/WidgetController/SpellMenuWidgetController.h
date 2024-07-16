// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSpellGlobeSelectedSignature, bool, bEnableSpendPointBtn, bool, bEnableEquipBtn, FString, Description, FString, NextLvlDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopWaitingEquipSelectionSignature);

struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag StatusTag = FGameplayTag();
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature OnSpellPointsChanged;
	
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void ShouldEnableButtons(const FGameplayTag& StatusTag, int32 InSpellPoints, bool& bEnableSpendPointBtn, bool& bEnableEquipBtn);

	// handles enabling/disabling buttons on the widget
	UPROPERTY(BlueprintAssignable)
	FOnSpellGlobeSelectedSignature OnSpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FStopWaitingEquipSelectionSignature StopWaitingForEquipDelegate;

	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();
	
private:
	FSelectedAbility SelectedAbility;
	int32 SpellPoints;
	bool bWaitingForEquipSelection = false;
};
