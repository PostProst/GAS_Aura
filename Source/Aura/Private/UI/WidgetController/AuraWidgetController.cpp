// P.S. project


#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraASC()->bStartupAbilitiesGiven) return;
	
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = UAuraAbilitySystemComponent::GetAbilityStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	GetAuraASC()->ForEachAbility(BroadcastDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	return AuraPlayerController == nullptr ? TObjectPtr<AAuraPlayerController>(CastChecked<AAuraPlayerController>(PlayerController)) : AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	return AuraPlayerState == nullptr ? TObjectPtr<AAuraPlayerState>(CastChecked<AAuraPlayerState>(PlayerState)) : AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
	return AuraAbilitySystemComponent == nullptr ? TObjectPtr<UAuraAbilitySystemComponent>(CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent)) : AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
	return AuraAttributeSet == nullptr ? TObjectPtr<UAuraAttributeSet>(CastChecked<UAuraAttributeSet>(AttributeSet)) : AuraAttributeSet;
}
