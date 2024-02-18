// P.S. project


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo)

	const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Attributes.Primary.Strength"));
	if (Tag.IsValid())
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
		Info.AttributeValue = AS->GetStrength();
		AttributeInfoDelegate.Broadcast(Info);
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	
}
