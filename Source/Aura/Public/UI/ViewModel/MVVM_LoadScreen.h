// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

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

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;


	// WidgetSwither's button callbacks
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);
	
private:
	
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlotsMap;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;


	// Field notify
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	int32 NumLoadSlots;

public:
	
	void SetNumLoadSlots(int32 InSlots) { UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots, InSlots); }
	int32 GetNumLoadSlots() const { return NumLoadSlots; }
};
