// P.S. project


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"


void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	// if save slot is already taken, override it
	DeleteSaveGameSlot(LoadSlot);
	
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName().ToString();
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->SaveSlotStatus = Taken;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), LoadSlot->LoadSlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	
	return Cast<ULoadScreenSaveGame>(SaveGameObject);
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;

	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveGameObject) const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;

	// Override latest checkpoint
	AuraGameInstance->PlayerStartTag = SaveGameObject->PlayerStartTag;
	
	UGameplayStatics::SaveGameToSlot(SaveGameObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	if (PlayerStarts.Num() > 0)
	{
		// set the PlayerStart to the first in the array just in case
		AActor* SelectedPlayerStart = PlayerStarts[0];
		for (AActor* Actor : PlayerStarts)
		{
			// loop over all player starts to find the one that's set on the GameInstance
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectedPlayerStart = PlayerStart;
					break;
				}
			}
		}
		return SelectedPlayerStart;
	}
	return nullptr;
}

void AAuraGameModeBase::SaveWorldState(UWorld* InWorld) const
{
	FString WorldName = InWorld->GetMapName();
	// removes streaming prefix from the WorldName so we can get the actual asset name
	WorldName.RemoveFromStart(InWorld->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGameInstance);
	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		// first time saving this map? - add a new one to the SavedMaps array
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.AddUnique(NewSavedMap);
		}
		// Create FSavedMap where we will override all FSavedActors.
		// We will replace the actual Map on the SaveGame object with it later
		FSavedMap SavedMap = SaveGame->GetSavedMapWIthMapName(WorldName);
		SavedMap.Actors.Empty(); // clear the array, we will replace all actors

		/* ------ Saving Actors' State ------
		* 1. iterate over all actors in the World
		* 2. find those who implement SaveGame interface
		* 3. save the actor itself FSavedActor->FSavedMap->ULoadScreenSaveGame
		* 4. serialize the Actor's UPROPERTY(SaveGame) variables
		*/
		for (FActorIterator It(InWorld); It; ++It)
		{
			AActor* Actor = *It;
			
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;
			
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			// serializing SaveGame variables
			FMemoryWriter MemoryWriter(SavedActor.Bytes); // Writer is for saving
			
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;
			//Archive.SetIsSaving(true);
			Actor->Serialize(Archive);

			SavedMap.Actors.AddUnique(SavedActor);
		}
		
		// MapToReplace& is a non-const ref.
		// we replace it with the SavedMap that we created and populated earlier
		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}
		
		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* InWorld) const
{
	FString WorldName = InWorld->GetMapName();
	WorldName.RemoveFromStart(InWorld->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGameInstance);
	
	if (UGameplayStatics::DoesSaveGameExist(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex));
		checkf(SaveGame, TEXT("Failed to load slot"));
		
		for (FActorIterator It(InWorld); It; ++It)
		{
			AActor* Actor = *It;
			
			if (!Actor->Implements<USaveInterface>()) continue;

			for (const FSavedActor& SavedActor : SaveGame->GetSavedMapWIthMapName(WorldName).Actors)
			{
				if (SavedActor.ActorName == Actor->GetFName())
				{
					// Update Actor's transform
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}
					
					// Deserialize all Actor's SaveGame variables
					FMemoryReader MemoryReader(SavedActor.Bytes); // Reader is for loading
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					//Archive.SetIsLoading(true);
					Actor->Serialize(Archive); // converts binary bytes back into variables

					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}

void AAuraGameModeBase::DeleteSaveGameSlot(UMVVM_LoadSlot* LoadSlot)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), LoadSlot->LoadSlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), LoadSlot->LoadSlotIndex);
	}
}
