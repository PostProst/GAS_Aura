// P.S. project


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	/** to get an Ability System of some actor we need to cast it to the IAbilitySystemInterface if the actor implements the interface
	 * 
	IAbilitySystemInterface* ASInterface = Cast<IAbilitySystemInterface>(Target);
	if (ASInterface)
	{
		UAbilitySystemComponent* ASC = ASInterface->GetAbilitySystemComponent();
	}
	*/

	// if the actor does not implement IAbilitySystemInterface, then it is best to use the static UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent function
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;
	check (GameplayEffectClass)
	// FGameplayEffectContextHandle is a wrapper for a FGameplayEffectContext
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this); // an object that the effect was created from
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);

	/** to call ApplyGameplayEffectSpecToSelf we need a const FGameplayEffectSpec& reference
	 *  so to get it from FGameplayEffectSpecHandle we need to get Data var from it which is a TSharedPtr
	 *  then we need to call Data.Get() to get a raw pointer* out of it
	 *  and after that we need to *dereference it
	*/
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	// Def - is definition. In other words it's the Gameplay Effect itself
	const bool bIsInfiniteEffect = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfiniteEffect && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		// if the effect is infinite, and its removal policy is set to remove on end overlap - add it to the map with the ASC
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	// remove infinite GE
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC =  UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		// fill in local TArray of handles to remove from the TMap later
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		
		// loop through all active effect handles and remove the one which ended overlap event with
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (HandlePair.Value == TargetASC)
			{
				// remove the effect from ASC
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				// add handle to the TArray to then remove it from the map
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}



