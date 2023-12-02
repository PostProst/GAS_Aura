// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

// 'Abstract' keyword means that the class is intended to be a base class for other classes
// and is not meant to be instantiated on its own.
UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category=Combat)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

private:	

	

};
