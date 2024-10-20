// P.S. project


#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/HighlightInterface.h"


AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = false;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("Magic Circle Decal");
	SetRootComponent(MagicCircleDecal);
	
	OverlapSphere = CreateDefaultSubobject<USphereComponent>("Overlap Sphere");
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetGenerateOverlapEvents(true);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}


void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AMagicCircle::OnSphereBeginOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AMagicCircle::OnSphereEndOverlap);
}

void AMagicCircle::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IHighlightInterface* EnemyInterface = Cast<IHighlightInterface>(OtherActor))
	{
		EnemyInterface->HighlightActor();
	}
}

void AMagicCircle::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IHighlightInterface* EnemyInterface = Cast<IHighlightInterface>(OtherActor))
	{
		EnemyInterface->UnHighlightActor();
	}
}

