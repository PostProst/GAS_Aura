// P.S. project


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		HitActor = HitResult.GetActor();
	}
	// cancel the ability if we didn't hit anything
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		if (CurrentActorInfo->PlayerController != nullptr)
		{
			OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		}
		if (CurrentActorInfo->AvatarActor != nullptr)
		{
			OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
		}	
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter)
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		const FVector WeaponLocation = ICombatInterface::Execute_GetWeapon(OwnerCharacter)->GetSocketLocation(FName("TipSocket"));
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwnerCharacter);
		FHitResult HitResult;
		UKismetSystemLibrary::SphereTraceSingle(
			OwnerCharacter,
			WeaponLocation,
			BeamTargetLocation,
			10.f,
			TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true);

		// reset MouseHitLocation and HitActor if there was someone between us and the desired hit target
		if (HitResult.bBlockingHit)
		{
			MouseHitLocation = HitResult.ImpactPoint;
			HitActor = HitResult.GetActor();
		}
		if (ICombatInterface* PrimaryTargetCombatInterface = Cast<ICombatInterface>(HitActor))
		{
			if(!PrimaryTargetCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
			{
				PrimaryTargetCombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);	
			}
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutTargets)
{
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCharacter);
	ActorsToIgnore.Add(HitActor);
	
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		AdditionalTargetsRadius,
		HitActor->GetActorLocation());

	// Amount of additional targets increases with ability lvl (no additional targets at lvl 1) and caps at MaxShockTargets
	const int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxShockTargets);

	UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutTargets, HitActor->GetActorLocation());

	// remove friends from the array
	TArray<AActor*> FriendTargets;
	for (AActor* Target : OutTargets)
	{
		if(UAuraAbilitySystemLibrary::IsFriend(OwnerCharacter, Target))
		{
			FriendTargets.AddUnique(Target);
		}
	}
	if (!FriendTargets.IsEmpty())
	{
		for (AActor* FriendTarget : FriendTargets)
		{
			AActor* FriendToRemove = nullptr;
			for (AActor* Target : OutTargets)
			{
				if (FriendTarget == Target)
				{
					FriendToRemove = FriendTarget;
				}
			}
			OutTargets.Remove(FriendToRemove);
		}
	}
	
	// bind to OnDeathDelegates for AdditionalTargets
	for (AActor* Target : OutTargets)
	{
		if (ICombatInterface* AdditionalTargetCombatInterface = Cast<ICombatInterface>(Target))
		{
			if(!AdditionalTargetCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				AdditionalTargetCombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);	
			}
		}
	}
}

void UAuraBeamSpell::ClearOnDeathDelegate(AActor* DeadActor)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(DeadActor))
	{
		CombatInterface->GetOnDeathDelegate().RemoveAll(this);
	}
}
