// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UDamageTextWidgetComponent;
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

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	/**
	 * Input
	 */
	UPROPERTY(EditDefaultsOnly, Category=Input)
	TObjectPtr<class UInputMappingContext> AuraContext;

	/* Input actions */ 
	UPROPERTY(EditDefaultsOnly, Category=Input)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category=Input)
	TObjectPtr<UInputAction> CtrlAction;
	/* end Input actions */ 
	
	/* Input callbacks */
	void Move(const FInputActionValue& InputActionValue);
	
	void CtrlPressed() { bCtrlKeyDown = true; };
	void CtrlReleased() { bCtrlKeyDown = false; };
	bool bCtrlKeyDown = false;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	/* end Input callbacks */

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetASC();
	
	FHitResult CursorHit;
	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;

	/* Click to move */
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
	/* end Click to move */

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UDamageTextWidgetComponent> DamageTextComponentClass;

};
