// P.S. project


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	checkf(AttributeInfo, TEXT("AttributeInfo is not set in the blueprint"))
	
	for (auto& Attribute : AttributeInfo->AttributeInformation)
	{
		BroadcastAttributeInfo(Attribute.AttributeTag);
	}
	
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnAttributePointsChanged.Broadcast(AuraPlayerState->GetAttributePoints());
	OnSpellPointsChanged.Broadcast(AuraPlayerState->GetSpellPoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	checkf(AttributeInfo, TEXT("AttributeInfo is not set in the blueprint"))
	
	for (auto& Attribute : AttributeInfo->AttributeInformation)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute.AttributeGetter).AddLambda(
	[this, Attribute](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Attribute.AttributeTag);
		});
	}
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			OnAttributePointsChanged.Broadcast(Points);
		}
	);
	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			OnSpellPointsChanged.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	if (AuraPlayerState->GetAttributePoints() <= 0) return;
	
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag) const
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
	Info.AttributeValue = Info.AttributeGetter.GetNumericValue(AS);
	AttributeInfoDelegate.Broadcast(Info);
}
