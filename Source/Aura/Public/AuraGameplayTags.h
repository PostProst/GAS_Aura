// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *  AuraGameplayTags
 *  Singleton (only one instance of the class exists)
 *  containing native Gameplay Tags
 */
struct FAuraGameplayTags
{
public:
 static const FAuraGameplayTags& Get() { return GameplayTags; }
 static void InitializeNativeGameplayTags();
 
protected:
 
private:
 static FAuraGameplayTags GameplayTags;
 
};
