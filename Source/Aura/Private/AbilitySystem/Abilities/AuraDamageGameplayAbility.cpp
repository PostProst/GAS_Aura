// P.S. project


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"


void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);

	// loop through the Damage Types map on the GA and assign SetByCallerMagnitude for each Damage Type 
	for (auto Pair : DamageTypes)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

void UAuraDamageGameplayAbility::SetVisibilityBasedAnimTickOption(EVisibilityBasedAnimTickOption Option)
{
	if (const ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (AvatarCharacter->GetMesh())
		{
			AvatarCharacter->GetMesh()->VisibilityBasedAnimTickOption = Option;
		}
	}
}

float UAuraDamageGameplayAbility::GetDamage(int32 Level, FGameplayTag DamageTypeTag)
{
	return DamageTypes.Find(DamageTypeTag)->GetValueAtLevel(Level);
}
