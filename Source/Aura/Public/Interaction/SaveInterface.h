// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ISaveInterface
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldLoadTransform() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadActor();
};