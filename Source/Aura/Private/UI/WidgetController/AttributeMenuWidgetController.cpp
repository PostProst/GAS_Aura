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
	
	OnAttributePointsChanged.Broadcast(GetAuraPS()->GetAttributePoints());
	OnSpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
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
	
	GetAuraPS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			OnAttributePointsChanged.Broadcast(Points);
		}
	);
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			OnSpellPointsChanged.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAuraPS()->GetAttributePoints() <= 0) return;
	
	GetAuraASC()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag)
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
	Info.AttributeValue = Info.AttributeGetter.GetNumericValue(GetAuraAS());
	AttributeInfoDelegate.Broadcast(Info);
}
