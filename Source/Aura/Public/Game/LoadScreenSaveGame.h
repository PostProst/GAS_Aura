// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString SlotName = FString();
	
	UPROPERTY()
	int32 SlotIndex = 0;

	/* Variables we want to save in a SaveGame object */
	
	UPROPERTY()
	FString PlayerName = FString("Default Name");

protected:


private:


	
};
