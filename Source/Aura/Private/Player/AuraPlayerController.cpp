// P.S. project


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Aura/Public/Input/AuraInputComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextWidgetComponent.h"
#include "Aura/Aura.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	MoveSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// assert that halts execution of the program if mapping context isn't valid
	check(AuraContext);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0.f);
	}
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// CastChecked is an assert that halts execution of the program if it fails
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(CtrlAction, ETriggerEvent::Started, this, &AAuraPlayerController::CtrlPressed);
	AuraInputComponent->BindAction(CtrlAction, ETriggerEvent::Completed, this, &AAuraPlayerController::CtrlReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle	= GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass, CursorHit.Location, FRotator::ZeroRotator);

		if (DecalMaterial)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();	
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.Location);
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextWidgetComponent* DamageText = NewObject<UDamageTextWidgetComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent(); // CreateDefaultSubobject() calls RegisterComponent() for us
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		// location on the Spline that is closest to the player
		const FVector LocationOnSpline = MoveSpline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = MoveSpline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); // yaw rotation of the controller

	// normalized forward and right direction vectors of the controller
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	// blocks cursor trace (when performing charged abilities for example)
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.CursorTrace"))))
	{
		UnhighlightActor(LastActor);
		UnhighlightActor(ThisActor);
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}
	// trace against ExcludePlayers channel when we've activated ArcaneShards ability, otherwise use Visibility channel for tracing
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	// LastActor - is the actor we were hovering over last frame
	LastActor = ThisActor;
	if (IsValid(CursorHit.GetActor()) && CursorHit.GetActor()->Implements<UHighlightInterface>())
	{
		ThisActor = CursorHit.GetActor();
	}
	else
	{
		ThisActor = nullptr;
	}
	
	// if both actors are valid and not the same we do nothing
	// as well as if they are both null
	if (LastActor != ThisActor)
	{
		UnhighlightActor(LastActor);
		HighlightActor(ThisActor);
	}
}

void AAuraPlayerController::HighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HighlightActor(InActor);
	}
}

void AAuraPlayerController::UnhighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnHighlightActor(InActor);
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.InputPressed"))))
	{
		return;
	}
	// if input is LMB (character is running)
	if (InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("InputTag.LMB"))))
	{
		if (IsValid(ThisActor))
		{
			TargetingStatus = ThisActor->Implements<UEnemyInterface>() ? TargetingEnemy : TargetingNonEnemy;
			bAutoRunning = false;
		}
		else
		{
			TargetingStatus = NotTargeting;
		}
	}
	if (GetASC())
	{
		GetASC()->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.InputReleased"))))
	{
		return;
	}
	// input is not LMB - try to activate ability
	if (!InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("InputTag.LMB"))))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);	
		}
		return;
	}

	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
	}
	
	// no target and Ctrl is not pressed - perform click to move
	if (TargetingStatus != TargetingEnemy && !bCtrlKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		// short press, not input hold
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// sets MoveToLocation for autorun if we clicked on the object that has a destination point
			if (IsValid(ThisActor) && ThisActor->Implements<UHighlightInterface>())
			{
				FVector Destination;
				if (IHighlightInterface::Execute_SetMoveToLocation(ThisActor, Destination))
				{
					CachedDestination = Destination;	
				}
			}
			// spawn click to move particle effect
			else if(GetASC() && !GetASC()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.InputPressed"))))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
			
			// get Navigation Path from the Character to the point under the cursor
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				MoveSpline->ClearSplinePoints();
				// add each point from the NavigationPath to our Move Spline
				for (auto& PointLoc : NavPath->PathPoints)
				{
					MoveSpline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					//DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Red, false, 3.f);
				}
				// always sets the destination to the last element of the PathPoints array if it wasn't calculated properly
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints.Last();
					bAutoRunning = true;
				}
			}
		}
		FollowTime = 0.f;
		TargetingStatus = NotTargeting;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.InputHeld"))))
	{
		return;
	}
	// input is not LMB - try to activate ability
	if (!InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("InputTag.LMB"))))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);	
		}
		return;
	}
	// if there's a target or Ctrl+LMB - try to activate ability
	if (TargetingStatus == TargetingEnemy || bCtrlKeyDown)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);	
		}
	}
	// no target, the Character is running
	// AddMovementInput each frame the Input is held
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		
		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		return AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}