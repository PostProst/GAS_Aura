// P.S. project


#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (auto AbilityInfo : AbilityInformation)
	{
		if (AbilityInfo.AbilityTag == AbilityTag)
		{
			return AbilityInfo;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find info for Tag %s on AbilityInfo %s"), *AbilityTag.GetTagName().ToString(), *GetNameSafe(this));
	}
	return FAuraAbilityInfo();
}
