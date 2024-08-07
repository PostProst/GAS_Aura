// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"


struct FGameplayTag;
class UAttributeInfo;
struct FAuraAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStatChangedSignature OnAttributePointsChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStatChangedSignature OnSpellPointsChanged;

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;
	
private:
	void BroadcastAttributeInfo(const FGameplayTag& Tag);
	
};
