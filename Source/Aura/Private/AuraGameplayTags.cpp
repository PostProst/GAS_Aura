// P.S. project


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

// static variables must be explicitly declared
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	// adding Gameplay Tags to global Tag Manager from native code
	UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("Reduced damage taken, improves Block Chance"));
}
