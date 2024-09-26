// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndexDelegate;
	
	void InitializeSlot();

	UPROPERTY()
	int32 LoadSlotIndex = 0;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus;

private:
	
	/* Field notifies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FText PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FString LoadSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter = "SetSelectSlotButtonEnabled", Getter = "GetSelectSlotButtonEnabled", meta=(AllowPrivateAccess=true))
	bool bEnableSelectSlotButton = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FString MapName;

public:
	
	void SetPlayerName(const FText& InPlayerName) { UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName); }
	FText GetPlayerName() const { return PlayerName; }

	void SetLoadSlotName(const FString& InLoadSlotName) { UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InLoadSlotName); }
	FString GetLoadSlotName() const { return LoadSlotName; }

	void SetSelectSlotButtonEnabled(const bool bIsEnabled) { UE_MVVM_SET_PROPERTY_VALUE(bEnableSelectSlotButton, bIsEnabled); }
	bool GetSelectSlotButtonEnabled() const { return bEnableSelectSlotButton; }

	void SetMapName(const FString& InMapName) { UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName); }
	FString GetMapName() const { return MapName; }
};
