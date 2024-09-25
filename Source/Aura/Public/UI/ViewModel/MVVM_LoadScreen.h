// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	void InitializeLoadSlotViewModels();

	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelectedDelegate;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;


	// WidgetSwither's button callbacks
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void BackButtonPressed();

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	void LoadData();
	
private:
	
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlotsMap;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	UPROPERTY()
	UMVVM_LoadSlot* SelectedLoadSlot;


	// Field notifies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter = "SetPlayAndDeleteButtonsEnabled", Getter = "GetPlayAndDeleteButtonsEnabled", meta=(AllowPrivateAccess=true))
	bool bEnablePlayAndDeleteButtons = false;

public:

	void SetPlayAndDeleteButtonsEnabled(const bool bEnabled) { UE_MVVM_SET_PROPERTY_VALUE(bEnablePlayAndDeleteButtons, bEnabled); }
	bool GetPlayAndDeleteButtonsEnabled() const { return bEnablePlayAndDeleteButtons; }
};
