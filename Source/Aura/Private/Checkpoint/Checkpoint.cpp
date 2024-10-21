// P.S. project


#include "Checkpoint/Checkpoint.h"
#include "Aura/Aura.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("Checkpoint Mesh");
	CheckpointMesh->SetupAttachment(RootComponent);
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CheckpointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BEIGE);
	CheckpointMesh->MarkRenderStateDirty();
	
	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(CheckpointMesh);
	OverlapSphere->SetGenerateOverlapEvents(true);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToLocation = CreateDefaultSubobject<USceneComponent>("MoveToLocation");
	MoveToLocation->SetupAttachment(GetRootComponent());
	
	bBindOverlapCallback = true;
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		HandleGlowEffect();
	}
}

void ACheckpoint::HighlightActor_Implementation()
{
	if (!bReached)
	{
		CheckpointMesh->SetRenderCustomDepth(true);
	}
}

void ACheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

bool ACheckpoint::SetMoveToLocation_Implementation(FVector& OutLocation)
{
	OutLocation = MoveToLocation->GetComponentLocation();
	return true;
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (bBindOverlapCallback)
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
	}
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		HandleGlowEffect();
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGM->SaveWorldState(GetWorld());
		}
	}
}

void ACheckpoint::HandleGlowEffect()
{
	// disable collision on the Sphere so we can't overlap with it more than once
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	UMaterialInstanceDynamic* DynamicMI = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMI);
	CheckpointReached(DynamicMI);
}
