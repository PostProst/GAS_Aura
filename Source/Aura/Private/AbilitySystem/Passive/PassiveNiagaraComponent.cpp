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
		ActivateIfEquipped(OwnerAuraASC);
	}
	// if the ASC is not yet valid, we get OnASCRegisteredDelegate from the CombatInterface
	// which is broadcast when ASC is registered, and bind to ActivatePassiveEffect after that
	else if (ICombatInterface* OwnerCombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		OwnerCombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(
		this, [this]
		(UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* OwnerAuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				OwnerAuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				ActivateIfEquipped(OwnerAuraASC);
			}
			
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

void UPassiveNiagaraComponent::ActivateIfEquipped(UAuraAbilitySystemComponent* OwnerAuraASC)
{
	// check if the passive ability was already given before we bound to the delegate
	if (OwnerAuraASC->bStartupAbilitiesGiven)
	{
		if (OwnerAuraASC->GetAbilityStatusFromTag(PassiveSpellTag).MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"))))
		{
			Activate();
		}
	}
}
