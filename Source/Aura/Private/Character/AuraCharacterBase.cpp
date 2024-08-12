// P.S. project


#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Debuff components */
	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnNiagaraComponent");
	BurnDebuffComponent->SetupAttachment(RootComponent);
	BurnDebuffComponent->DebuffTag = FGameplayTag::RequestGameplayTag(FName("Debuff.Burn"));

	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunNiagaraComponent");
	StunDebuffComponent->SetupAttachment(RootComponent);
	StunDebuffComponent->DebuffTag = FGameplayTag::RequestGameplayTag(FName("Debuff.Stun"));
	/* end debuff components */
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsInShock);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	if (WeaponMesh)
	{
		return WeaponMesh;
	}
	return nullptr;
}

void AAuraCharacterBase::BeginDestroy()
{
	Super::BeginDestroy();
	// THIS IS IMPORTANT!
	// release SoftObjectPtr assets from the memory, otherwise they stay in memory FOREVER
	if (DissolveMaterialInstanceHandle.IsValid() && DissolveMaterialInstanceHandle->IsActive())
	{
		DissolveMaterialInstanceHandle->CancelHandle();
	}
	if (WeaponDissolveMaterialInstanceHandle.IsValid() && WeaponDissolveMaterialInstanceHandle->IsActive())
	{
		WeaponDissolveMaterialInstanceHandle->CancelHandle();
	}
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::OnRep_Stunned()
{
}

void AAuraCharacterBase::OnRep_Burned()
{
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// return SocketLocation depending on the Montage Tag
FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	if(MontageTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("CombatSocket.Weapon"))))
	{
		check(WeaponMesh);
        return WeaponMesh->GetSocketLocation(WeaponTipSocketName);
	}
	if(MontageTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("CombatSocket.RightHand"))))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if(MontageTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("CombatSocket.LeftHand"))))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if(MontageTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("CombatSocket.Tail"))))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
	
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContextHandle);
	EffectContextHandle.AddSourceObject(this);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes);
	ApplyEffectToSelf(DefaultSecondaryAttributes);
	ApplyEffectToSelf(DefaultVitalAttributes);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority()) return;
	
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	// Async load dissolve materials for character and weapon mesh and start dissolve timeline (implemented in BP)
	LoadDissolveMaterial(DissolveMaterialInstance, DissolveMaterialInstanceHandle, GetMesh());
	LoadDissolveMaterial(WeaponDissolveMaterialInstance, WeaponDissolveMaterialInstanceHandle, WeaponMesh);
}

void AAuraCharacterBase::LoadDissolveMaterial(TSoftObjectPtr<UMaterialInstance> MaterialInst,
	TSharedPtr<FStreamableHandle>& MaterialHandle, USkeletalMeshComponent* SkeletalMesh)
{
	// RequestAsyncLoad takes a TSoftObjectPtr to load (also need to call .ToSoftObjectPath()) and a callback which is called when the asset is loaded
	// RequestAsyncLoad returns a FStreamableHandle where we store the asset in a global var, so we can later free the memory manually (in BeginDestroy)
	MaterialHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
	MaterialInst.ToSoftObjectPath(),
	FStreamableDelegate::CreateLambda(
		[this, MaterialInst, SkeletalMesh]()
		{
			if (MaterialInst.IsValid())
			{
				UMaterialInstanceDynamic* DynamicDissolveMaterial = UMaterialInstanceDynamic::Create(MaterialInst.Get(), this);
				SkeletalMesh->SetMaterial(0, DynamicDissolveMaterial);
				if (SkeletalMesh == GetMesh())
				{
					StartDissolveTimeline(DynamicDissolveMaterial);
				}
				if (SkeletalMesh == WeaponMesh)
				{
					StartWeaponDissolveTimeline(DynamicDissolveMaterial);
				}
			}
		})
	);
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpulse);
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;	
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (const FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	bDead = true;

	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	WeaponMesh->AddImpulse(DeathImpulse * 0.01f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Dissolve();

	OnDeathDelegate.Broadcast(this);
}



