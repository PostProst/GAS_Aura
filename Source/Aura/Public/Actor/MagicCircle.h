// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

class USphereComponent;

UCLASS()
class AURA_API AMagicCircle : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AMagicCircle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> MagicCircleDecal;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> OverlapSphere;
	
private:	

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
