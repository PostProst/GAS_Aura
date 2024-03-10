// P.S. project


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	// Macro creates FGameplayEffectAttributeCaptureDefinition for a given Attribute
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
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
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluationParameters;
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Get Damage Set by Caller Magnitude
	float OutgoingDamage = Spec.GetSetByCallerMagnitude(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage")));

	// Capture BlockChance from the Target
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f); // Maxing the value between itself and 0, so it never becomes negative

	// If there was a successful Block, if so - half the Damage
	const float RandomNumber = FMath::FRandRange(UE_SMALL_NUMBER, 100.f);
	if (TargetBlockChance >= RandomNumber)
	{
		OutgoingDamage = OutgoingDamage * 0.5f;
	}
	
	// Capture Armor from the Target
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	// Capture Armor Penetration from the Source
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	// Query DamageCoefficients from the CharacterClass DataAsset to use in damage calculations
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);

	// Query ArmorPenetration coefficient from the Source
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	// finds a value on the curve at a specified level
	const float ArmorPenCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

	// ArmorPenetration ignores a percentage of the Target's armor (ArmorPenetration is scaled by ArmorPenCoefficient)
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenCoefficient) / 100.f;

	// Query EffectiveArmorCoefficient from the Target
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	
	// Incoming damage is reduced by the EffectiveArmor percent which is scaled by EffectiveArmorCoefficient
	OutgoingDamage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// OutExecutionOutput - is the outparameter struct which is used for modifying the attribute
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, OutgoingDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
