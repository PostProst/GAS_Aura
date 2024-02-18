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

struct FAuraGameplayTags
{
public:
 static const FAuraGameplayTags& Get() { return GameplayTags; }
 static void InitializeNativeGameplayTags();

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
 
protected:
 
private:
 static FAuraGameplayTags GameplayTags;
 
};