// P.S. project


#include "AbilitySystem/Abilities/HomingFireball.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"


void UHomingFireball::SpawnHomingProjectiles(const FVector& TargetLocation, AActor* TargetActor,
                                             const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) return;
	
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator ProjectileRotation = (TargetLocation - SocketLocation).Rotation();

	// makes the Projectile parallel to the ground
	bOverridePitch ? ProjectileRotation.Pitch = PitchOverride : ProjectileRotation.Pitch;
	
	// get number of projectiles to spawn from the curve asset depending on the Ability level
	const int32 NumProjectiles = ProjectilesCurve.GetRichCurveConst()->Eval(GetAbilityLevel());
	
	const FVector Forward = ProjectileRotation.Vector();
	
	TArray<FRotator> ProjectileRotations = UAuraAbilitySystemLibrary::GetEvenlySpacedRotators(
		Forward,
		FVector::UpVector,
		ProjectileSpread,
		NumProjectiles);

	// Spawn Projectiles
	for(const FRotator& Rotation : ProjectileRotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()), 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		if (TargetActor && TargetActor->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = TargetActor->GetRootComponent();
		}
		else
		{
			// if we didn't click on the enemy, create a scene component dynamically
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(TargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}
		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		
		Projectile->FinishSpawning(SpawnTransform);
		
	}
}

void UHomingFireball::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
