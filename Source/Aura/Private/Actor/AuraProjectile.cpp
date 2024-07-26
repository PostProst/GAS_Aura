// P.S. project


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Aura/Aura.h"


AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}


void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	bHit = true;
}

void AAuraProjectile::Destroyed()
{
	// case when the Projectile is destroyed on the server before OnSphereOverlap happened on the client
	if (!bHit && !HasAuthority())
	{
		OnHit();
	}
	Super::Destroyed();
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	UGameplayStatics::SpawnSoundAttached(LoopingSound, Sphere, NAME_None, Sphere->GetComponentLocation(), EAttachLocation::KeepRelativeOffset, true);
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bHit) OnHit();
	
	if (HasAuthority())
	{
		AActor* SourceAvatarActor = DamageEffectParams.SourceASC->GetAvatarActor();
	
		// prevents overlapping with self
		if(SourceAvatarActor == OtherActor) return;
		// prevents friendly fire
		if (UAuraAbilitySystemLibrary::IsFriend(SourceAvatarActor, OtherActor)) return;
		
		// apply damage GameplayEffect
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetASC = TargetASC;
			
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;

			FRotator Rotation = GetActorRotation();
			Rotation.Pitch = 45.f;
			const FVector KnockbackImpulse = Rotation.Vector() * DamageEffectParams.KnockbackMagnitude;
			DamageEffectParams.KnockbackImpulse = KnockbackImpulse;
			
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		
		Destroy();
	}
	else bHit = true;
}



