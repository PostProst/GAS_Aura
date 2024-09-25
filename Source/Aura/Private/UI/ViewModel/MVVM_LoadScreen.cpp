// P.S. project


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlotViewModels()
{
	checkf(LoadSlotViewModelClass, TEXT("LoadSlotViewModelClass is not set in MVVM Load Screen blueprint"));
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlot_0->LoadSlotIndex = 0;
	LoadSlotsMap.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlot_1->LoadSlotIndex = 1;
	LoadSlotsMap.Add(1, LoadSlot_1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlot_2->LoadSlotIndex = 2;
	LoadSlotsMap.Add(2, LoadSlot_2);
	
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlotsMap.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	LoadSlotsMap[Slot]->SaveSlotStatus = Taken;
	AuraGameMode->SaveSlotData(LoadSlotsMap[Slot], Slot);
	LoadSlotsMap[Slot]->InitializeSlot();
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlotsMap[Slot]->SetWidgetSwitcherIndexDelegate.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	// enabling/disabling select slot buttons
	for (const auto& Pair : LoadSlotsMap)
	{
		if (Pair.Key == Slot)
		{
			Pair.Value->SetSelectSlotButtonEnabled(false);
		}
		else
		{
			Pair.Value->SetSelectSlotButtonEnabled(true);
		}
	}
	SelectedLoadSlot = LoadSlotsMap[Slot];
	SetPlayAndDeleteButtonsEnabled(true);
}

void UMVVM_LoadScreen::BackButtonPressed()
{
	for (const auto& Pair : LoadSlotsMap)
	{
		Pair.Value->SetSelectSlotButtonEnabled(true);
	}
	SetPlayAndDeleteButtonsEnabled(false);
	SelectedLoadSlot = nullptr;
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (IsValid(SelectedLoadSlot))
	{
		AAuraGameModeBase::DeleteSaveGameSlot(SelectedLoadSlot);
		SelectedLoadSlot->SaveSlotStatus = Vacant;
		SelectedLoadSlot->SetSelectSlotButtonEnabled(true);
		SelectedLoadSlot->InitializeSlot();
	}
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	
	for (const auto& Pair : LoadSlotsMap)
	{
		ULoadScreenSaveGame* SaveGameObject = AuraGameMode->GetSaveSlotData(Pair.Value->GetLoadSlotName(), Pair.Key);

		Pair.Value->SaveSlotStatus = SaveGameObject->SaveSlotStatus;
		Pair.Value->SetPlayerName(FText::FromString(SaveGameObject->PlayerName));
		Pair.Value->InitializeSlot();
	}
}
