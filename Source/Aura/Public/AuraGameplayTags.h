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

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Resistance_Fire);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Resistance_Lightning);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Resistance_Arcane);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Attributes_Resistance_Physical);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_LMB);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_RMB);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_3);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_InputTag_4);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage_Fire);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage_Lightning);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage_Arcane);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Damage_Physical);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Effects_HitReact);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Abilities_Attack);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Montage_Attack_Weapon);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Montage_Attack_RightHand);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Montage_Attack_LeftHand);

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
 
protected:
 
private:
 static FAuraGameplayTags GameplayTags;
 
};
