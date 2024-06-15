// P.S. project


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 GivenXP) const
{
	bool bSearching = true;
	int32 Level = 1;
	while (bSearching)
	{
		// LevelUpInformation[1] = info for level 1, 0th element in this array is just a placeholder and doesn't contain any valuable data
		if (LevelUpInformation.Num() - 1 <= Level) return Level;
	
		if (GivenXP >= LevelUpInformation[Level].ExpRequirement)
		{
			Level++;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
