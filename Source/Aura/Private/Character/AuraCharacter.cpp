// P.S. project


#include "Character/AuraCharacter.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Commandlets/WorldPartitionCommandletHelpers.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->bDoCollisionTest = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(RootComponent);
	LevelUpNiagaraComponent->bAutoActivate = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// InitAbilityActorInfo for the server
	InitAbilityActorInfo();
	LoadProgress();
}

void AAuraCharacter::LoadProgress()
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;
		
		if (SaveData->bFirstTimeLoading)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{
			AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
			check(AuraPlayerState);
			AuraPlayerState->SetLevel(SaveData->PlayerLevel);
			AuraPlayerState->SetXP(SaveData->XP);
			AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
			AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
			UAuraAbilitySystemLibrary::InitializeAttributesFromSaveData(this, GetAbilitySystemComponent(), SaveData);
			// TODO load abilities from disk
		}
	}
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// InitAbilityActorInfo for the client
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	if (AuraPlayerState->GetSpellPoints() > 0)
	{
		AuraPlayerState->AddToSpellPoints(InSpellPoints);	
	}
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Controller))
	{
		AuraPC->ShowMagicCircle(DecalMaterial);
		AuraPC->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Controller))
	{
		AuraPC->HideMagicCircle();
		AuraPC->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;
		
		SaveData->PlayerStartTag = CheckpointTag;

		AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
		check(AuraPlayerState);
		SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
		SaveData->XP = AuraPlayerState->GetXP();
		SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
		SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
		
		UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(GetAttributeSet());
		check(AuraAttributeSet);
		SaveData->Strength = AuraAttributeSet->GetStrength();
		SaveData->Intelligence = AuraAttributeSet->GetIntelligence();
		SaveData->Resilience = AuraAttributeSet->GetResilience();
		SaveData->Vigor = AuraAttributeSet->GetVigor();

		SaveData->bFirstTimeLoading = false;

		// -------Saving Abilities-------
		// 1. Create FForEachAbility delegate
		// 2. Bind Lambda to it. There we create FSavedAbility struct and save all Ability data to it
		// 3. Add FSavedAbility struct to the array of abilities on the SaveGame object
		// 4. Call FForEachAbility on ASC to execute this labmda on every activatable ability
		FForEachAbility SaveAbilityDelegate;
		SaveAbilityDelegate.BindLambda([this, SaveData](const FGameplayAbilitySpec& Spec)
		{
			FSavedAbility SavedAbility;
			SavedAbility.Ability = Spec.Ability.GetClass();
			SavedAbility.AbilityLevel = Spec.Level;
			SavedAbility.AbilityStatus = UAuraAbilitySystemComponent::GetAbilityStatusFromSpec(Spec);
			SavedAbility.AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(Spec);
			SavedAbility.AbilityInputSlot = UAuraAbilitySystemComponent::GetInputTagFromSpec(Spec);
			SavedAbility.AbilityType = UAuraAbilitySystemComponent::GetAbilityTypeFromSpec(Spec);

			SaveData->Abilities.Add(SavedAbility);
		});
		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		AuraASC->ForEachAbility(SaveAbilityDelegate);
		
		AuraGameMode->SaveInGameProgressData(SaveData);
	}
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	check(LevelUpNiagaraComponent);
	const FVector CameraLocation = CameraComponent->GetComponentLocation();
	const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
	const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
	LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
	LevelUpNiagaraComponent->Activate(true);
}

void AAuraCharacter::OnLevelUp(int32 NewLevel, bool bLevelUp)
{
	if (bLevelUp)
	{
		MulticastLevelUpParticles();	
	}
}

// As dynamic gameplay effects are not replicated when we create debuff GE in AttributeSet
// we need to disable input on clients in OnRep_Stunned
void AAuraCharacter::OnRep_Stunned()
{
	FGameplayTagContainer BlockedTags;
	BlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.InputPressed")));
	BlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.InputHeld")));
	BlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.InputReleased")));
	BlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Block.CursorTrace")));
	if(bIsStunned)
	{
		AbilitySystemComponent->AddLooseGameplayTags(BlockedTags);
		StunDebuffComponent->Activate();
	}
	else
	{
		AbilitySystemComponent->RemoveLooseGameplayTags(BlockedTags);
		StunDebuffComponent->Deactivate();
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FGameplayTag::RequestGameplayTag(FName("Debuff.Stun")),
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);
	AttributeSet = AuraPlayerState->GetAttributeSet();
	AuraPlayerState->OnLevelChangedDelegate.AddUObject(this, &AAuraCharacter::OnLevelUp);

	if (AAuraPlayerController* AuraPlayerController = GetController<AAuraPlayerController>())
	{
		if (AAuraHUD* AuraHUD = AuraPlayerController->GetHUD<AAuraHUD>())
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	/* Initialize Primary and Secondary Attributes with a Gameplay Effect */
	//InitializeDefaultAttributes();
}

void AAuraCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);

}

