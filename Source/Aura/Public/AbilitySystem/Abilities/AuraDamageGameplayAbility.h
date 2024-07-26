// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilitySystemTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

protected:

	// sets a flag on the Mesh to correctly update bone positions on the server when playing AnimMontages
	UFUNCTION(BlueprintCallable)
	void SetVisibilityBasedAnimTickOption(EVisibilityBasedAnimTickOption Option);

	// meta=(Categories="Damage") - limits GameplayTags selection in editor only to ones that start with 'Damage'
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage", meta=(Categories="Damage"))
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FScalableFloat Damage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float DebuffDamage = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float DebuffDuration = 5.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DeathImpulseMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float KnockbackChance = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float KnockbackMagnitude = 1000.f;
	

	UFUNCTION(BlueprintPure)
	float GetDamage(int32 Level, FGameplayTag DamageTypeTag);
	
};
