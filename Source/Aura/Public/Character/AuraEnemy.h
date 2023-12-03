// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Aura/Public/Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	
	// interact interface functions
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	
};
