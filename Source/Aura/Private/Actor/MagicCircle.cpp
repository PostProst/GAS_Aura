// P.S. project


#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"


AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = false;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("Magic Circle Decal");
	SetRootComponent(MagicCircleDecal);
	//MagicCircleDecal->SetupAttachment(RootComponent);
}


void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
	
}


void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

