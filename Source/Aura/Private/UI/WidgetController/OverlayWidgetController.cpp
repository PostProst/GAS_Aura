// P.S. project


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	/* Binding callbacks to the ASC's value changed delegates which broadcast our delegates to widgets */
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		// Binding a callback for a AbilitiesGivenDelegate, broadcast when abilities are given on startup if they weren't given yet
		if (!AuraASC->bStartupAbilitiesGiven)
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}
		// call the callback directly if the abilities were already given
		else
		{
			OnInitializeStartupAbilities(AuraASC);
		}
		
		/* EffectAssetTagsDelegate is broadcast in Aura ASC when any Effect is applied
         * here we bind Lambda callback to this delegate to check all Tags gained from the applied Effect and find Message Tags
         * then we broadcast our MessageWidgetRowDelegate where we search our Data Table (defined in BP) for the required Tag by name
         * to show the message and other related info in WBP_EffectMessage
         * (the callback for MessageWidgetRowDelegate is assigned and defined in WBP_Overlay)
         */
        AuraASC->EffectAssetTagsDelegate.AddLambda(
        [this](const FGameplayTagContainer& AssetTags)
	        {
        		for (const auto& Tag : AssetTags)
        		{
        			/* for example: Tag = Message.HealthPotion
        			 * "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False */
        			
        			FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
        			if (Tag.MatchesTag(MessageTag))
        			{
        				const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
        				MessageWidgetRowDelegate.Broadcast(*Row);				
        			}
        		}
	        }
        );
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;
	
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

