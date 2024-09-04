// P.S. project


#include "AbilitySystem/Abilities/AuraFireBlast.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireball.h"

TArray<AAuraFireball*> UAuraFireBlast::SpawnFireballs()
{
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::GetEvenlySpacedRotators(
		GetAvatarActorFromActorInfo()->GetActorForwardVector(),
		FVector::UpVector,
		360.f,
		NumFireBalls
		);

	TArray<AAuraFireball*> Fireballs;
	
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetAvatarActorFromActorInfo()->GetActorLocation());
		SpawnTransform.SetRotation(Rotator.Quaternion());
		
		AAuraFireball* Fireball = GetWorld()->SpawnActorDeferred<AAuraFireball>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

		Fireball->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		Fireballs.Add(Fireball);

		Fireball->FinishSpawning(SpawnTransform);
		
	}
	
	return Fireballs;
}
