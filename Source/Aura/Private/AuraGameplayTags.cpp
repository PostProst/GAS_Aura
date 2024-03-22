// P.S. project


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Vital_Health, "Attributes.Vital.Health");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Vital_Mana, "Attributes.Vital.Mana");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Attributes_Primary_Strength, "Attributes.Primary.Strength", "Increases physical damage");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Attributes_Primary_Intelligence, "Attributes.Primary.Intelligence", "Increases magical damage");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Attributes_Primary_Resilience, "Attributes.Primary.Resilience", "Increases armor and armor penetration");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tag_Attributes_Primary_Vigor, "Attributes.Primary.Vigor", "Increases health");

UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_Armor, "Attributes.Secondary.Armor");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_ArmorPenetration, "Attributes.Secondary.ArmorPenetration");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_BlockChance, "Attributes.Secondary.BlockChance");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_CriticalHitChance, "Attributes.Secondary.CriticalHitChance");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_CriticalHitDamage, "Attributes.Secondary.CriticalHitDamage");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_CriticalHitResistance, "Attributes.Secondary.CriticalHitResistance");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_HealthRegeneration, "Attributes.Secondary.HealthRegeneration");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_ManaRegeneration, "Attributes.Secondary.ManaRegeneration");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_MaxHealth, "Attributes.Secondary.MaxHealth");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Secondary_MaxMana, "Attributes.Secondary.MaxMana");

UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Resistance_Fire, "Attributes.Resistance.Fire");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Resistance_Lightning, "Attributes.Resistance.Lightning");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Resistance_Arcane, "Attributes.Resistance.Arcane");
UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Resistance_Physical, "Attributes.Resistance.Physical");

UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_LMB, "InputTag.LMB");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_RMB, "InputTag.RMB");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_1, "InputTag.1");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_2, "InputTag.2");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_3, "InputTag.3");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_4, "InputTag.4");

UE_DEFINE_GAMEPLAY_TAG(Tag_Damage, "Damage");
UE_DEFINE_GAMEPLAY_TAG(Tag_Damage_Fire, "Damage.Fire");
UE_DEFINE_GAMEPLAY_TAG(Tag_Damage_Lightning, "Damage.Lightning");
UE_DEFINE_GAMEPLAY_TAG(Tag_Damage_Arcane, "Damage.Arcane");
UE_DEFINE_GAMEPLAY_TAG(Tag_Damage_Physical, "Damage.Physical");

UE_DEFINE_GAMEPLAY_TAG(Tag_Effects_HitReact, "Effects.HitReact");

UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Attack, "Abilities.Attack");

UE_DEFINE_GAMEPLAY_TAG(Tag_Montage_Attack_Weapon, "Montage.Attack.Weapon");
UE_DEFINE_GAMEPLAY_TAG(Tag_Montage_Attack_RightHand, "Montage.Attack.RightHand");
UE_DEFINE_GAMEPLAY_TAG(Tag_Montage_Attack_LeftHand, "Montage.Attack.LeftHand");

// static variables must be explicitly defined
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

// Second way of adding Gameplay Tags from native manually without using macros
void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/*	add a new Gameplay Tag to global Gameplay Tags Manager from native code
	 *	store created Tag in local associated FGameplayTag variable ->
	 *	on the only existing FAuraGameplayTags::GameplayTags instance of this class
	 
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("Reduced damage taken, improves Block Chance"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"), FString("Ignores percentage of enemy Armor, increases Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"), FString("Chance to cut incoming damage in half"));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"), FString("Chance to double outgoing damage plus hit bonus"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"), FString("Bonus damage added when Critical Hit is scored"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"), FString("Reduces Critical Hit Chance of attacking enemies"));
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegeneration"), FString("Amount of Health regenerated every 1 second"));
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ManaRegeneration"), FString("Amount of Mana regenerated every 1 second"));
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"), FString("Maximum amount of Health obtainable"));
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"), FString("Maximum amount of Mana obtainable"));
	*/
	
	// Map Damage Types to Resistances
	GameplayTags.DamageTypesToResistances.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage.Fire")), UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Fire")));
	GameplayTags.DamageTypesToResistances.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage.Lightning")), UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Lightning")));
	GameplayTags.DamageTypesToResistances.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage.Arcane")), UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Arcane")));
	GameplayTags.DamageTypesToResistances.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage.Physical")), UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Physical")));
}
