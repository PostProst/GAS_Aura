// P.S. project


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContext, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
	if(AuraGameMode == nullptr) return;
	
	// get the Data Asset from the game mode
	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	checkf(CharacterClassInfo, TEXT("Set CharacterClassInfo on the GameMode"));
	// get CharacterClassDefaultInfo struct corresponding to the character class
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	
	// handle applying gameplay effects to initialize default Primary, Secondary and Vital attributes

	// set SourceObject needed for GameplayModMagnitudeCalculation
	const AActor* SourceObject = ASC->GetAvatarActor();
	
	// context handle to pass in all ApplyGameplayEffectSpecToSelf
	FGameplayEffectContextHandle PrimaryEffectContextHandle = ASC->MakeEffectContext();
	PrimaryEffectContextHandle.AddSourceObject(SourceObject);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryEffectContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryEffectContextHandle = ASC->MakeEffectContext();
	SecondaryEffectContextHandle.AddSourceObject(SourceObject);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryEffectContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalEffectContextHandle = ASC->MakeEffectContext();
	VitalEffectContextHandle.AddSourceObject(SourceObject);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalEffectContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
	
}
