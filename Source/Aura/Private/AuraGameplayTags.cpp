// P.S. project


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

// static variables must be explicitly declared
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/*	add a new Gameplay Tag to global Gameplay Tags Manager from native code
	 *	store created Tag in local associated FGameplayTag variable ->
	 *	on the only existing FAuraGameplayTags::GameplayTags instance of this class
	 */ 
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("Reduced damage taken, improves Block Chance"));
	
}
