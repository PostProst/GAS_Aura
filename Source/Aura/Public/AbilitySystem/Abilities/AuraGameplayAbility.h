// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;

	UFUNCTION(BlueprintImplementableEvent)
	FString GetDescription(int32 InLevel);
	
	UFUNCTION(BlueprintImplementableEvent)
	FString GetNextLvlDescription(int32 InLevel);
	
	static FString GetLockedDescription(int32 InLevel);

	UFUNCTION(BlueprintPure, meta = (HidePin = "Target"))
	float GetCooldown(int32 InLevel = 1) const;

	UFUNCTION(Blueprintpure, meta = (HidePin = "Target"))
	float GetManaCost(int32 InLevel = 1) const;
		
	
};
