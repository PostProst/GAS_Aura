// P.S. project


#include "Actor/PointCollection.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


APointCollection::APointCollection()
{
	
	PrimaryActorTick.bCanEverTick = false;

	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePoints.Add(Pt_0);
	SetRootComponent(Pt_0);

	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");
	ImmutablePoints.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());

	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	ImmutablePoints.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());

	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	ImmutablePoints.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());

	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	ImmutablePoints.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());

	Pt_5= CreateDefaultSubobject<USceneComponent>("Pt_5");
	ImmutablePoints.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());

	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	ImmutablePoints.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());

	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	ImmutablePoints.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());

	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	ImmutablePoints.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());
	
	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	ImmutablePoints.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());

	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	ImmutablePoints.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, const int32 NumPoints, float YawOverride)
{
	checkf(ImmutablePoints.Num() >= NumPoints, TEXT("Attempted to access ImmutablePoints out of bounds"));
	
	// here we create a local TArray<USceneComponent*> to return,
	// it is populated with copies of ImmutablePoints with adjusted transforms,
	TArray<USceneComponent*> PointsCopy;
	for (auto Pt : ImmutablePoints)
	{
		// as soon as our local array reaches needed NumPoints - return it
		if (PointsCopy.Num() >= NumPoints) return PointsCopy;
		
		if (Pt != Pt_0) // do not rotate the root
		{
			FVector ToPoint  = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}

		// trace from above to find a point on uneven ground
		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 500.f);

		// we need to ignore all character actors for the trace
		TArray<AActor*> IgnoreActors;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
			this,
			IgnoreActors,
			TArray<AActor*>(),
			500.f,
			GetActorLocation());

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), QueryParams);

		const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
		
		Pt->SetWorldLocation(AdjustedLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		PointsCopy.Add(Pt);
	}
	return PointsCopy;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}


