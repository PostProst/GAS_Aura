// P.S. project


#include "Actor/AuraFireball.h"

void AAuraFireball::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraFireball::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}
