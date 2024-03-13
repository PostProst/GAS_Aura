// P.S. project


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemTypes.h"
#include "AuraGameplayTags.h"
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
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
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
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
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
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	FAggregatorEvaluateParameters EvaluationParameters;
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Local Damage variable
	float OutgoingDamage = 0.f;
	// Loop through all DamageTypes defined in FAuraGameplayTags singleton
	// and add its value corresponding to its Tag (if there's any) to the local OutgoingDamage var 
	for (FGameplayTag DamageTypeTag : FAuraGameplayTags::Get().DamageTypes)
	{
		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag);
		OutgoingDamage += DamageTypeValue;
	}
	
	// Capture BlockChance from the Target
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f); // Maxing the value between itself and 0, so it never becomes negative

	// If there was a successful Block, if so - half the Damage
	const float RandomBlockNumber = FMath::FRandRange(UE_SMALL_NUMBER, 100.f);
	if (TargetBlockChance >= RandomBlockNumber)
	{
		UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, true);
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


	// Critical Hit calculation
	float SourceCritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCritChance);
	SourceCritChance = FMath::Max<float>(SourceCritChance, 0.f);

	float TargetCritResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCritResistance);
	TargetCritResistance = FMath::Max<float>(TargetCritResistance, 0.f);
	
	float SourceCritDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCritDamage);
	SourceCritDamage = FMath::Max<float>(SourceCritDamage, 0.f);

	// Query CriticalHitResistanceCoefficient from the Target
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	// CriticalHitChance is reduced by scaled CriticalHitResistance
	const float EffectiveCritHitChance = SourceCritChance - TargetCritResistance * CriticalHitResistanceCoefficient;

	// Critical Hit = x2 damage + CriticalHitDamage
	const float RandomCritNumber = FMath::FRandRange(UE_SMALL_NUMBER, 100.f);
	if (EffectiveCritHitChance >= RandomCritNumber)
	{
		UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, true);
		OutgoingDamage = OutgoingDamage * 2.f + SourceCritDamage;
	}

	// OutExecutionOutput - is the outparameter struct which is used for modifying the attribute
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, OutgoingDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
