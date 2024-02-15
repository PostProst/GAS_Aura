// P.S. project


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = WorldContextObject->GetWorld()->GetFirstPlayerController())
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			FWidgetControllerParams WidgetControllerParams;
			WidgetControllerParams.PlayerController = PC;
			WidgetControllerParams.PlayerState = PC->GetPlayerState<AAuraPlayerState>();
			WidgetControllerParams.AttributeSet = PC->GetPlayerState<AAuraPlayerState>()->GetAttributeSet();
			WidgetControllerParams.AbilitySystemComponent = PC->GetPlayerState<AAuraPlayerState>()->GetAbilitySystemComponent();
			
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = WorldContextObject->GetWorld()->GetFirstPlayerController())
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			FWidgetControllerParams WidgetControllerParams;
			WidgetControllerParams.PlayerController = PC;
			WidgetControllerParams.PlayerState = PC->GetPlayerState<AAuraPlayerState>();
			WidgetControllerParams.AttributeSet = PC->GetPlayerState<AAuraPlayerState>()->GetAttributeSet();
			WidgetControllerParams.AbilitySystemComponent = PC->GetPlayerState<AAuraPlayerState>()->GetAbilitySystemComponent();
			
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}
