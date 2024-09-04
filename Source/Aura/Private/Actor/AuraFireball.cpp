// P.S. project


#include "Actor/AuraFireball.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void AAuraFireball::OnHit()
{
	// When we have some entity that is already replicated
	// (in this case Fireball is replicated and OnHit() is called in AuraProjectile's Destroyed()),
	// it is a good practice to execute NonReplicated GameplayCues to save on bandwidth and do not use unnecessary RPCs.
	// Execution of these GameplayCues will still be propagated to all clients
	if(IsValid(GetInstigator()))
	{
		FGameplayCueParameters Params;
		Params.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetInstigator(), FGameplayTag::RequestGameplayTag(FName("GameplayCue.FireBlast")), Params);
	}
	
	bHit = true;
}

void AAuraFireball::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireball::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (HasAuthority())
	{
		if (!IsValidOverlap(OtherActor)) return;
		
		// prevent multiple overlaps with the same actor
		if (IgnoreActors.Contains(OtherActor)) return;
		
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			IgnoreActors.Add(OtherActor);
			
			DamageEffectParams.TargetASC = TargetASC;
			
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
