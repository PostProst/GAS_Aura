// P.S. project


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"


void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch)
{
	// we can then pass the address of ActivationInfo (&) to HasAuthority to satisfy pointer input parameter
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) return;

	// if an avatar actor inherits from ICombatInterface
	// find weapon socket location of the spawned projectile
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator ProjectileRotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	// makes the Projectile parallel to the ground
	bOverridePitch ? ProjectileRotation.Pitch = 0.f : ProjectileRotation.Pitch;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(ProjectileRotation.Quaternion());

	// SpawnActorDeferred returns a pointer to an object of a spawned class
	// it gives an opportunity to set needed parameters on the Actor
	// after that it is mandatory to manually call FinishSpawning
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	Projectile->FinishSpawning(SpawnTransform);
}

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
