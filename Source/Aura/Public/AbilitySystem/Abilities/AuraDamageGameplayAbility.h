// P.S. project

#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION(BlueprintPure)
	float GetDamage(int32 Level, FGameplayTag DamageTypeTag);
	
};
