// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "HomingFireball.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UHomingFireball : public UAuraProjectileSpell
{
	GENERATED_BODY()

protected:
	
	UFUNCTION(BlueprintCallable)
	void SpawnHomingProjectiles(const FVector& TargetLocation, AActor* TargetActor, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride);

	UPROPERTY(EditDefaultsOnly, Category="Homing Projectile")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="Homing Projectile")
	FRuntimeFloatCurve ProjectilesCurve;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category="Homing Projectile")
	float HomingAccelerationMin = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="Homing Projectile")
	float HomingAccelerationMax = 2400.f;

	UPROPERTY(EditDefaultsOnly, Category="Homing Projectile")
	bool bLaunchHomingProjectiles = true;
private:
};
