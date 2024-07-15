// P.S. project


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"


FString UAuraGameplayAbility::GetLockedDescription(int32 InLevel)
{
	return FString::Printf(TEXT("<Default>Spell unlocks at Level </><Level>%d</>"), InLevel);
}

float UAuraGameplayAbility::GetCooldown(int32 InLevel) const
{
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		float CooldownDuration = 0;
		if (CooldownGE->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, CooldownDuration))
		{
			return CooldownDuration;
		}
	}
	return 0;
}

float UAuraGameplayAbility::GetManaCost(int32 InLevel) const
{
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		for (auto Modifier : CostGE->Modifiers)
		{
			if (Modifier.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				float ManaCost = 0.f;
				if (Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost))
				{
					return ManaCost;
				}
			}
		}
	}
	return 0;
}
