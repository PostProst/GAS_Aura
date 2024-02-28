// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

struct FInputActionValue;
class UInputAction;
class IEnemyInterface;
class UAuraInputComponent;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	
	virtual void PlayerTick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	/**
	 * Input
	 */
	UPROPERTY(EditDefaultsOnly, Category=Input)
	TObjectPtr<class UInputMappingContext> AuraContext;

	// input actions 
	UPROPERTY(EditDefaultsOnly, Category=Input)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category=Input)
	TObjectPtr<UInputAction> CtrlAction;
	
	// input callbacks
	void Move(const FInputActionValue& InputActionValue);
	
	void CtrlPressed() { bCtrlKeyDown = true; };
	void CtrlReleased() { bCtrlKeyDown = false; };
	bool bCtrlKeyDown = false;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetASC();
	
	FHitResult CursorHit;
	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;

	// Click to move
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> MoveSpline;


};
