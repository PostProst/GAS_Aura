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

UE_DEFINE_GAMEPLAY_TAG(Tag_Attributes_Meta_IncomingXP, "Attributes.Meta.IncomingXP");

UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_LMB, "InputTag.LMB");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_RMB, "InputTag.RMB");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_1, "InputTag.1");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_2, "InputTag.2");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_3, "InputTag.3");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_4, "InputTag.4");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_Passive_1, "InputTag.Passive.1");
UE_DEFINE_GAMEPLAY_TAG(Tag_InputTag_Passive_2, "InputTag.Passive.2");

UE_DEFINE_GAMEPLAY_TAG(Tag_Damage, "Damage");
UE_DEFINE_GAMEPLAY_TAG(Tag_Damage_Fire, "Damage.Fire");
UE_DEFINE_GAMEPLAY_TAG(Tag_Damage_Lightning, "Damage.Lightning");
UE_DEFINE_GAMEPLAY_TAG(Tag_Damage_Arcane, "Damage.Arcane");
UE_DEFINE_GAMEPLAY_TAG(Tag_Damage_Physical, "Damage.Physical");

UE_DEFINE_GAMEPLAY_TAG(Tag_Debuff_Burn, "Debuff.Burn");
UE_DEFINE_GAMEPLAY_TAG(Tag_Debuff_Stun, "Debuff.Stun");
UE_DEFINE_GAMEPLAY_TAG(Tag_Debuff_Arcane, "Debuff.Arcane");
UE_DEFINE_GAMEPLAY_TAG(Tag_Debuff_Physical, "Debuff.Physical");
UE_DEFINE_GAMEPLAY_TAG(Tag_Debuff_Chance, "Debuff.Chance");
UE_DEFINE_GAMEPLAY_TAG(Tag_Debuff_Damage, "Debuff.Damage");
UE_DEFINE_GAMEPLAY_TAG(Tag_Debuff_Frequency, "Debuff.Frequency");
UE_DEFINE_GAMEPLAY_TAG(Tag_Debuff_Duration, "Debuff.Duration");

UE_DEFINE_GAMEPLAY_TAG(Tag_Knockback_Chance, "Knockback.Chance");

UE_DEFINE_GAMEPLAY_TAG(Tag_Effects_HitReact, "Effects.HitReact");

UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_None, "Abilities.None");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Attack, "Abilities.Attack");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Summon, "Abilities.Summon");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_HitReact, "Abilities.HitReact");

UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Status_Locked, "Abilities.Status.Locked");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Status_Eligible, "Abilities.Status.Eligible");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Status_Unlocked, "Abilities.Status.Unlocked");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Status_Equipped, "Abilities.Status.Equipped");

UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Type_Offensive, "Abilities.Type.Offensive");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Type_Passive, "Abilities.Type.Passive");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Type_None, "Abilities.Type.None");

UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Fire_FireBolt, "Abilities.Fire.FireBolt");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Fire_HomingFireballs, "Abilities.Fire.HomingFireballs");
UE_DEFINE_GAMEPLAY_TAG(Tag_Abilities_Lightning_Electrocute, "Abilities.Lightning.Electrocute");

UE_DEFINE_GAMEPLAY_TAG(Tag_Cooldown_Fire_FireBolt, "Cooldown.Fire.FireBolt");

UE_DEFINE_GAMEPLAY_TAG(Tag_CombatSocket_Weapon, "CombatSocket.Weapon");
UE_DEFINE_GAMEPLAY_TAG(Tag_CombatSocket_RightHand, "CombatSocket.RightHand");
UE_DEFINE_GAMEPLAY_TAG(Tag_CombatSocket_LeftHand, "CombatSocket.LeftHand");
UE_DEFINE_GAMEPLAY_TAG(Tag_CombatSocket_Tail, "CombatSocket.Tail");

UE_DEFINE_GAMEPLAY_TAG(Tag_Montage_Attack_1, "Montage.Attack.1");
UE_DEFINE_GAMEPLAY_TAG(Tag_Montage_Attack_2, "Montage.Attack.2");
UE_DEFINE_GAMEPLAY_TAG(Tag_Montage_Attack_3, "Montage.Attack.3");
UE_DEFINE_GAMEPLAY_TAG(Tag_Montage_Attack_4, "Montage.Attack.4");

UE_DEFINE_GAMEPLAY_TAG(Tag_Player_Block_InputPressed, "Player.Block.InputPressed");
UE_DEFINE_GAMEPLAY_TAG(Tag_Player_Block_InputHeld, "Player.Block.InputHeld");
UE_DEFINE_GAMEPLAY_TAG(Tag_Player_Block_InputReleased, "Player.Block.InputReleased");
UE_DEFINE_GAMEPLAY_TAG(Tag_Player_Block_CursorTrace, "Player.Block.CursorTrace");

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
	
	// map DamageTypes to Resistances
	GameplayTags.DamageTypesToResistances.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage.Fire")), UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Fire")));
	GameplayTags.DamageTypesToResistances.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage.Lightning")), UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Lightning")));
	GameplayTags.DamageTypesToResistances.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage.Arcane")), UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Arcane")));
	GameplayTags.DamageTypesToResistances.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName("Damage.Physical")), UGameplayTagsManager::Get().RequestGameplayTag(FName("Attributes.Resistance.Physical")));

	// map DamageTypes to Debuffs
	GameplayTags.DamageTypesToDebuffs.Add(FGameplayTag::RequestGameplayTag(FName("Damage.Fire")), FGameplayTag::RequestGameplayTag(FName("Debuff.Burn")));
	GameplayTags.DamageTypesToDebuffs.Add(FGameplayTag::RequestGameplayTag(FName("Damage.Lightning")), FGameplayTag::RequestGameplayTag(FName("Debuff.Stun")));
	GameplayTags.DamageTypesToDebuffs.Add(FGameplayTag::RequestGameplayTag(FName("Damage.Arcane")), FGameplayTag::RequestGameplayTag(FName("Debuff.Arcane")));
	GameplayTags.DamageTypesToDebuffs.Add(FGameplayTag::RequestGameplayTag(FName("Damage.Physical")), FGameplayTag::RequestGameplayTag(FName("Debuff.Physical")));
}
