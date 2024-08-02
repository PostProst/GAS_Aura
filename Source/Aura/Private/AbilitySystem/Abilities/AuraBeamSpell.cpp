// P.S. project


#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "GameFramework/Character.h"

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
