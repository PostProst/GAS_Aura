// P.S. project


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	// here we want to bind to ActivatePassiveEffect on the Owner's AbilitySystemComponent
	// first, we try to get it from the owner directly, but it may not yet be valid
	if (UAuraAbilitySystemComponent* OwnerAuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		OwnerAuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
	}
	// if the ASC is not yet valid, we get OnASCRegisteredDelegate from the CombatInterface
	// which is broadcast when ASC is registered, and bind to ActivatePassiveEffect after that
	else if (ICombatInterface* OwnerCombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		OwnerCombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(
			this, [this]
			(UAbilitySystemComponent* ASC)
			{
				Cast<UAuraAbilitySystemComponent>(ASC)->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
			});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();	
		}
	}
}
