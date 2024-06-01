// P.S. project


#include "AbilitySystem/Data/AttributeInfo.h"
#include "Aura/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag == Tag)
		{
			return Info;
		}
	}
	// Log an error if the Tag wasn't found and bLogNotFound is set to true
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find Info for AttributeTag %s on AttributeInfo %s"), *Tag.ToString(), *GetNameSafe(this));
	}
	// return an empty struct if the required Tag wasn't found
	return FAuraAttributeInfo();
}
