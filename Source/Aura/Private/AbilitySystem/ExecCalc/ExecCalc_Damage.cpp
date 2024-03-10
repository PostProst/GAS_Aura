// P.S. project


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "AbilitySystem/AuraAttributeSet.h"

struct AuraDamageStatics
{
	// Macro creates FGameplayEffectAttributeCaptureDefinition for a given Attribute
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	
	AuraDamageStatics()
	{
		/* Macro defines the CaptureDefinition struct for a given attribute
		 * S - AttributeSet class where the Attribute is defined
		 * P - Attribute to capture
		 * T - EGameplayEffectAttributeCaptureSource enum has 2 sources to capture the Attribute from: Source and Target
		 * B - bool bSnapshot if true, captures the Attributes when Effect Spec is created, otherwise when applied
		 */ 
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
	}
};

// static func to create return static AuraDamageStatics struct
static const AuraDamageStatics& DamageStatics()
{
	// static member variables declared in a static func will always return the same object
	static AuraDamageStatics Dstatics;
	
	return  Dstatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Add the AttributeCaptureDefinition we created to the array of AttributesToCapture
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluationParameters;
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Get Damage Set by Caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage")));

	// Capture BlockChance on target, and determine if there was a successful Block, if so - half the Damage
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	
	const float RandomNumber = FMath::FRandRange(UE_SMALL_NUMBER, 100.f);
	if (TargetBlockChance >= RandomNumber)
	{
		Damage = Damage * 0.5f;
	}
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
