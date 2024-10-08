// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/**
 *  AuraGameplayTags singleton (only one instance of the class exists)
 *  containing native Gameplay Tags
 *  
 **********************************
 *
 *  One way of adding gameplay tags from native is using 2 macros:
 *  UE_DECLARE_GAMEPLAY_TAG_EXTERN in .h and
 *  UE_DEFINE_GAMEPLAY_TAG() or UE_DEFINE_GAMEPLAY_TAG_COMMENT in .cpp
 *  these 2 macros MUST be used together
 */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Vital_Health);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Vital_Mana);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Primary_Strength);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Primary_Intelligence);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Primary_Resilience);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Primary_Vigor);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_Armor);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_ArmorPenetration);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_BlockChance);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_CriticalHitChance);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_CriticalHitDamage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_CriticalHitResistance);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_HealthRegeneration);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_ManaRegeneration);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_MaxHealth);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Secondary_MaxMana);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Meta_IncomingXP);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_LMB);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_RMB);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_3);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_4);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_Passive_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_Passive_2);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage_Fire);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage_Lightning);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage_Arcane);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage_Physical);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Resistance_Fire);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Resistance_Lightning);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Resistance_Arcane);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Resistance_Physical);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Debuff_Burn);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Debuff_Stun);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Debuff_Arcane);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Debuff_Physical);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Debuff_Chance);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Debuff_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Debuff_Frequency);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Debuff_Duration);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Knockback_Chance);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Effects_HitReact);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_None);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Attack);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Summon);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_HitReact);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Status_Locked);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Status_Eligible);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Status_Unlocked);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Status_Equipped);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Type_Offensive);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Type_Passive);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Type_None);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Fire_FireBolt);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Fire_HomingFireballs);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Fire_FireBlast);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Lightning_Electrocute);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Arcane_ArcaneShards);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Passive_HaloOfProtection);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Passive_LifeSiphon);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Passive_ManaSiphon);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Cooldown_Fire_FireBolt);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_CombatSocket_Weapon);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_CombatSocket_RightHand);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_CombatSocket_LeftHand);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_CombatSocket_Tail);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Montage_Attack_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Montage_Attack_2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Montage_Attack_3);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Montage_Attack_4);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Player_Block_InputPressed);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Player_Block_InputHeld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Player_Block_InputReleased);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Player_Block_CursorTrace);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_GameplayCue_FireBlast);

struct FAuraGameplayTags
{
public:
 static const FAuraGameplayTags& Get() { return GameplayTags; }
 static void InitializeNativeGameplayTags();

 /*
 FGameplayTag Attributes_Secondary_Armor;
 FGameplayTag Attributes_Secondary_ArmorPenetration;
 FGameplayTag Attributes_Secondary_BlockChance;
 FGameplayTag Attributes_Secondary_CriticalHitChance;
 FGameplayTag Attributes_Secondary_CriticalHitDamage;
 FGameplayTag Attributes_Secondary_CriticalHitResistance;
 FGameplayTag Attributes_Secondary_HealthRegeneration;
 FGameplayTag Attributes_Secondary_ManaRegeneration;
 FGameplayTag Attributes_Secondary_MaxHealth;
 FGameplayTag Attributes_Secondary_MaxMana;
 */
 
 TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
 TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;
 
protected:
 
private:
 static FAuraGameplayTags GameplayTags;
 
};
