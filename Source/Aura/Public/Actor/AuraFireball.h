// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireball.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraFireball : public AAuraProjectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	// Additional DamageEffectParams for exploding on the way back
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;

	virtual void OnHit() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> IgnoreActors;

	
private:
	
	
};
