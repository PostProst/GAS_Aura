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
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	
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
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		// Map Attribute Gameplay Tags to Attribute Capture Definitions
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Secondary.Armor")), ArmorDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Secondary.ArmorPenetration")), ArmorPenetrationDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Secondary.BlockChance")), BlockChanceDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Secondary.CriticalHitChance")), CriticalHitChanceDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Secondary.CriticalHitDamage")), CriticalHitDamageDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Secondary.CriticalHitResistance")), CriticalHitResistanceDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Fire")), FireResistanceDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Lightning")), LightningResistanceDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Arcane")), ArcaneResistanceDef);
		TagsToCaptureDefs.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Physical")), PhysicalResistanceDef);
	}
};

// static func to create return static AuraDamageStatics struct
static const AuraDamageStatics& GetDamageStatics()
{
	// static member variables declared in a static func will always return the same object
	static AuraDamageStatics Dstatics;
	
	return  Dstatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Add the AttributeCaptureDefinition we created to the array of AttributesToCapture
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	int32 SourceLevel = 1;
	int32 TargetLevel = 1;
	// Implements<>() function needs to take in U version of the interface class instead of I
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourceLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	FAggregatorEvaluateParameters EvaluationParameters;
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Local Damage variable
	float Damage = 0.f;
	
	// Loop through DamageTypesToResistances Tag Map defined in FAuraGameplayTags singleton
	for (const auto& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;

		checkf(GetDamageStatics().TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs Map doesn't contain Tag: %s in ExecCalc_Damage"), *ResistanceTag.ToString());

		// Find Attribute Capture Definition corresponding to the Resistance Tag
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = GetDamageStatics().TagsToCaptureDefs[ResistanceTag];

		// Get the damage associated with the Damage Type Tag from the AbilitySpec
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);

		// Capture Resistance Attribute Magnitude
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		// Reduce the DamageTypeValue by Resistance percent
		DamageTypeValue *= (100.f - Resistance) / 100.f;

		// Set the total damage value
		Damage += DamageTypeValue;
	}
	
	// Capture BlockChance from the Target
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f); // Maxing the value between itself and 0, so it never becomes negative

	// If there was a successful Block, if so - half the Damage
	const float RandomBlockNumber = FMath::FRandRange(UE_SMALL_NUMBER, 100.f);
	if (TargetBlockChance >= RandomBlockNumber)
	{
		UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, true);
		Damage = Damage * 0.5f;
	}
	
	// Capture Armor from the Target
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	// Capture Armor Penetration from the Source
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	// Query DamageCoefficients from the CharacterClass DataAsset to use in damage calculations
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);

	// Query ArmorPenetration coefficient from the Source
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	// finds a value on the curve at a specified level
	const float ArmorPenCoefficient = ArmorPenetrationCurve->Eval(SourceLevel);

	// ArmorPenetration ignores a percentage of the Target's armor (ArmorPenetration is scaled by ArmorPenCoefficient)
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenCoefficient) / 100.f;

	// Query EffectiveArmorCoefficient from the Target
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetLevel);
	
	// Incoming damage is reduced by the EffectiveArmor percent which is scaled by EffectiveArmorCoefficient
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;


	// Critical Hit calculation
	float SourceCritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCritChance);
	SourceCritChance = FMath::Max<float>(SourceCritChance, 0.f);

	float TargetCritResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCritResistance);
	TargetCritResistance = FMath::Max<float>(TargetCritResistance, 0.f);
	
	float SourceCritDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCritDamage);
	SourceCritDamage = FMath::Max<float>(SourceCritDamage, 0.f);

	// Query CriticalHitResistanceCoefficient from the Target
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetLevel);

	// CriticalHitChance is reduced by scaled CriticalHitResistance
	const float EffectiveCritHitChance = SourceCritChance - TargetCritResistance * CriticalHitResistanceCoefficient;

	// Critical Hit = x2 damage + CriticalHitDamage
	const float RandomCritNumber = FMath::FRandRange(UE_SMALL_NUMBER, 100.f);
	if (EffectiveCritHitChance >= RandomCritNumber)
	{
		UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, true);
		Damage = Damage * 2.f + SourceCritDamage;
	}

	// OutExecutionOutput - is the outparameter struct which is used for modifying the attribute
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
