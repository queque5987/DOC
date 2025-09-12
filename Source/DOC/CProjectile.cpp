#include "CProjectile.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/IDamagable.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IEnemyCharacter.h"
#include "DrawDebugHelpers.h"

ACProjectile::ACProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Collider->SetSphereRadius(15.f);
	SetRootComponent(SceneComponent);
}

void ACProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	CollisionQueryParam.AddIgnoredActor(this);
}

void ACProjectile::Fire(FDamageConfig DamageConfig, FVector Direction, float Velocity, float Range)
{
	Config = DamageConfig;
	Dir = Direction;
	Vel = Velocity;
	Trail = 0.f;
	MaxRange = Range;
	SetActorRotation(Direction.Rotation());
	SetActorTickEnabled(true);
	if (ParticleSystemComponent != nullptr) ParticleSystemComponent->Activate();
}

void ACProjectile::Fire(FDamageConfig DamageConfig, float Velocity, USplineComponent* FollowTrace)
{
	SplineComponent = FollowTrace;
	Config = DamageConfig;
	Vel = Velocity;
	MaxRange = FollowTrace->GetSplineLength();
}

void ACProjectile::SetParticleSystemComponent(UParticleSystemComponent* PSC)
{
	ParticleSystemComponent = PSC;
}

void ACProjectile::SetObjectPoolManager(IIObjectPoolManager* OPM)
{
	ObjectPoolManager = OPM;
}

void ACProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector PostLocation = SplineComponent != nullptr ?
		SplineComponent->GetLocationAtDistanceAlongSpline(Trail, ESplineCoordinateSpace::World) :
		GetActorLocation() + Dir * Vel * DeltaTime;


	TArray<FHitResult> HitResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(6.f);
	GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), PostLocation, FQuat(), ECollisionChannel::ECC_Pawn, CollisionShape, CollisionQueryParam);
	SetActorLocation(PostLocation);
	for (const FHitResult& HitResult : HitResults)
	{
		IIDamagable* DMG = Cast<IIDamagable>(HitResult.GetActor());
		if (DMG != nullptr && HitResult.GetActor() != Config.Causer)
		{
			if (HitResult.GetActor()->Implements<UIEnemyCharacter>() == Config.Causer->Implements<UIEnemyCharacter>()) continue;
			SetActorTickEnabled(false);
			if (ParticleSystemComponent != nullptr) ParticleSystemComponent->Deactivate();
			ParticleSystemComponent = nullptr;
			Config.HitLocation = HitResult.ImpactPoint;
			Config.HitDirection = Dir;
			Config.CausedTimeSeconds = GetWorld()->TimeSeconds;
			Config.AttackType = ATTACK_TYPE_RANGED;
			Config.DamageWidgetColor = DAMAGE_COLOR_MINION;
			DMG->RecieveDamage(Config);
			if (ObjectPoolManager != nullptr) ObjectPoolManager->ReturnProjectile(this);
		}
	}
	Trail += Vel * DeltaTime;

	if (Trail >= MaxRange)
	{
		SetActorTickEnabled(false);
		ParticleSystemComponent->Deactivate();
		ParticleSystemComponent = nullptr;
	}
}

