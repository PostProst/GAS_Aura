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
	
	// input callbacks
	void Move(const FInputActionValue& InputActionValue);

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetASC();
	
	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;




};
