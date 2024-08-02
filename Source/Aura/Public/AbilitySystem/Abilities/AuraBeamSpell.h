// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	
protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Beam")
	FVector MouseHitLocation;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite, Category="Beam")
	TObjectPtr<AActor> HitActor;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
	
private:
	
};
