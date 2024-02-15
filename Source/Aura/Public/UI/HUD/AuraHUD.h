// P.S. project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UAuraUserWidget;
class UOverlayWidgetController;
struct FWidgetControllerParams;
class UAttributeMenuWidgetController;

/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	// template function for creating (if not existing) and returning any type of Widget Controller
	template <typename T>
	FORCEINLINE T* GetWidgetController(TObjectPtr<T> WidgetController, TSubclassOf<T> WidgetControllerClass, const FWidgetControllerParams& WCParams)
	{
		if(WidgetController == nullptr)
		{
			WidgetController = NewObject<T>(this, WidgetControllerClass);
			WidgetController->SetWidgetControllerParams(WCParams);
			WidgetController->BindCallbacksToDependencies();
		}
		return WidgetController;
	}

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:


	
private:

	// Overlay Widget

	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	// AttributeMenuWidgetController
	
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};
