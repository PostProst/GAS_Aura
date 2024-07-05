// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

class UAttributeSet;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
	
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Abilities")
	FAbilityInfoSignature AbilityInfoDelegate;
	
	void BroadcastAbilityInfo();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Data")
	TObjectPtr<class UAbilityInfo> AbilityInfo;

	// base engine classes
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<UAttributeSet> AttributeSet;

	
	// project specific classes
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<class AAuraPlayerController> AuraPlayerController;

	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<class AAuraPlayerState> AuraPlayerState;

	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<class UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<class UAuraAttributeSet> AuraAttributeSet;

	AAuraPlayerController* GetAuraPC();
	AAuraPlayerState* GetAuraPS();
	UAuraAbilitySystemComponent* GetAuraASC();
	UAuraAttributeSet* GetAuraAS();
	
};
