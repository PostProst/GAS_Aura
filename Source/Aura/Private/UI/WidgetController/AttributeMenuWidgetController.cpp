// P.S. project


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	checkf(AttributeInfo, TEXT("AttributeInfo is not set in the blueprint"))
	
	for (auto& Attribute : AttributeInfo->AttributeInformation)
	{
		BroadcastAttributeInfo(Attribute.AttributeTag);
	}
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
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag) const
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
	Info.AttributeValue = Info.AttributeGetter.GetNumericValue(AS);
	AttributeInfoDelegate.Broadcast(Info);
}
