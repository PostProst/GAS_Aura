// P.S. project


#include "UI/HUD/MainMenuHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	checkf(ViewModelClass, TEXT("ViewModelClass is not set in MainMenuHUD blueprint"));
	ViewModel = NewObject<UMVVM_LoadScreen>(this, ViewModelClass);
	ViewModel->InitializeLoadSlotViewModels();
	
	checkf(LoadScreenWidgetClass, TEXT("LoadScreenWidgetClass is not set in MainMenuHUD blueprint"));
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();
	LoadScreenWidget->BlueprintInitializeWidget();

	ViewModel->LoadData();
}
