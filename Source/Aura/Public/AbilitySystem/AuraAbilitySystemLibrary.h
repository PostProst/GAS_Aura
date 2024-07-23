// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"


struct FDamageEffectParams;
struct FGameplayEffectContextHandle;
class UAbilitySystemComponent;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
struct FWidgetControllerParams;
class AAuraHUD;
class UAbilityInfo;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary | WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD);

	// Widget Controller getters
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary | WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary | WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary | WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);


	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary | CharacterClassDefaults", meta = (DefaultToSelf = "WorldContext"))
	static void InitializeDefaultAttributes(const UObject* WorldContext, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary | CharacterClassDefaults", meta = (DefaultToSelf = "WorldContext"))
	static void GiveStartupAbilities(const UObject* WorldContext, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary | CharacterClassDefaults", meta = (DefaultToSelf = "WorldContext"))
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContext);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary | Ability Info", meta = (DefaultToSelf = "WorldContext"))
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary | CharacterClassDefaults", meta = (DefaultToSelf = "WorldContext"))
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContext, ECharacterClass Class, int32 Level);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary | GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary | GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	//UPARAM(ref) - makes a non const reference parameter an input in the blueprint
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary | GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary | GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary | GameplayMechanics", meta = (DefaultToSelf = "WorldContext"))
	static void GetLivePlayersWithinRadius(const UObject* WorldContext, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary | GameplayMechanics")
	static bool IsFriend(const AActor* SourceActor, const AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary | DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);
};
