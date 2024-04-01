// P.S. project


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"


void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// we can then pass the address of ActivationInfo (&) to HasAuthority to satisfy pointer input parameter
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) return;

	// if an avatar actor inherits from ICombatInterface
	// find weapon socket location of the spawned projectile
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), FGameplayTag::RequestGameplayTag(FName("CombatSocket.Weapon")));
	FRotator ProjectileRotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	ProjectileRotation.Pitch = 0.f; // makes the Projectile parallel to the ground

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(ProjectileRotation.Quaternion());

	// SpawnActorDeferred returns a pointer to an object of a spawned class
	// it gives an opportunity to set needed parameters on the Actor
	// after that is is mandatory to manually call FinishSpawning
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// set the damage GameplayEffectSpecHandle on the projectile
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this); // sets AbilityCDO and Level
	EffectContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);
	
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	
	for (auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		// AssignTagSetByCallerMagnitude assigns a pair of GameplayTag to Magnitude
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}
	
	Projectile->DamageEffectSpecHandle = SpecHandle;
	
	Projectile->FinishSpawning(SpawnTransform);
}

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
