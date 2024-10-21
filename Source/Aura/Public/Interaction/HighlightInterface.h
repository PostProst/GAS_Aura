// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IHighlightInterface
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintNativeEvent)
	void HighlightActor();
	
	UFUNCTION(BlueprintNativeEvent)
	void UnHighlightActor();

	// returns a special point in space that the player will move to (staircases, entrances etc.)
	UFUNCTION(BlueprintNativeEvent)
	bool SetMoveToLocation(FVector& OutLocation);
};
