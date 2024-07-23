#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilitySystemTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_USTRUCT_BODY()

public:
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return  bIsBlockedHit; }

	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	
	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
protected:

	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;
	
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()
	
	FDamageEffectParams(){}

	UPROPERTY()
	TObjectPtr<UObject> WorldContext = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	float BaseDamage = 0.f;
	float AbilityLevel = 1.f;
	
	FGameplayTag DamageType = FGameplayTag();

	float DebuffChance = 0.f;
	float DebuffDamage = 0.f;
	float DebuffDuration = 0.f;
	float DebuffFrequency = 0.f;
};

/** type traits to cover the custom aspects of a script struct **/
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	/* enums (treated as bools) allow the struct of type FAuraGameplayEffectContext to:
	 * @WithNetSerializer - have a NetSerialize function for serializing its state to an FArchive used for network replication
	 * @WithCopy - to be copied via its copy assignment operator
	 */ 
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
