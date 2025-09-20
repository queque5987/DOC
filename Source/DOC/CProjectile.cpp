#include "CProjectile.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/IDamagable.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IEnemyCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ACProjectile::ACProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Collider->SetSphereRadius(15.f);
	SetRootComponent(SceneComponent);
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void ACProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	CollisionQueryParam.AddIgnoredActor(this);
}

void ACProjectile::Fire(FDamageConfig DamageConfig, FVector Direction, float Velocity, float Range)
{
	bUseSplineGuide = false;
	Config = DamageConfig;
	Dir = Direction;
	Vel = Velocity;
	Trail = 0.f;
	Target = nullptr;
	MaxRange = Range;
	SetActorRotation(Direction.Rotation());
	SetActorTickEnabled(true);
	if (ParticleSystemComponent != nullptr) ParticleSystemComponent->Activate();
}

void ACProjectile::Fire(FDamageConfig DamageConfig, float Velocity, USplineComponent* FollowTrace, AActor* TargetActor)
{
	bUseSplineGuide = true;
	if (FollowTrace == nullptr) return;
	Target = TargetActor;
	if (SplineComponent != nullptr)
	{
		SplineComponent->SetWorldTransform(FollowTrace->GetComponentTransform());
		SplineComponent->SplineCurves = FollowTrace->SplineCurves;
		SplineComponent->UpdateSpline();
	}
	Config = DamageConfig;
	Vel = Velocity;
	if (Target != nullptr) Dir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal2D();
	MaxRange = FollowTrace->GetSplineLength() * 2.5f;
	SetActorTickEnabled(true);
	Trail = 0.f;
}

void ACProjectile::Fire(FDamageConfig DamageConfig, float Velocity, FRotator InitRotation, AActor* TargetActor)
{
	bUseSplineGuide = false;
	Target = TargetActor;
	Config = DamageConfig;
	Vel = Velocity;
	if (Target != nullptr) Dir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal2D();
	MaxRange = FVector::Dist(TargetActor->GetActorLocation(), GetActorLocation()) * 5.f;
	SetActorRotation(InitRotation);
	SetActorTickEnabled(true);
	Trail = 0.f;
	DynamicRotDegPerTick = MaxRotDegPerTick * 20.f;
	if (DamageConfig.ProjectileSpawnedEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(DamageConfig.ProjectileSpawnedEffect, SceneComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true, EPSCPoolMethod::AutoRelease);
	}
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

	FVector PostLocation = GetActorLocation();
	if (bUseSplineGuide && SplineComponent != nullptr)	// Spline Guided
	{
		if (Target != nullptr) SplineComponent->SetLocationAtSplinePoint(2, Target->GetActorLocation(), ESplineCoordinateSpace::World);

		FVector SplineGuideDir = (SplineComponent->GetLocationAtDistanceAlongSpline(Trail, ESplineCoordinateSpace::World) - GetActorLocation()).GetSafeNormal();
		FVector ForwardDir = GetActorForwardVector();
		FVector LaunchDir = ForwardDir;
		float Rad = FMath::Acos(FVector::DotProduct(ForwardDir, SplineGuideDir));
		if (FMath::Abs(FMath::RadiansToDegrees(Rad)) <= MaxRotDegPerTick)
		{
			LaunchDir = SplineGuideDir;
		}
		else
		{
			FVector RotAxis = FVector::CrossProduct(ForwardDir, SplineGuideDir).GetSafeNormal();
			LaunchDir = ForwardDir.RotateAngleAxis(MaxRotDegPerTick, RotAxis);
		}
		//PostLocation = GetActorLocation() + LaunchDir * Vel * DeltaTime;
		Dir = LaunchDir;
		//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + LaunchDir * 300.f, 50.f, FColor::Green);
		//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + SplineGuideDir * 300.f, 50.f, FColor::Red);
		//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForwardDir * 300.f, 50.f, FColor::Blue);
		//DrawDebugSphere(GetWorld(), SplineComponent->GetLocationAtDistanceAlongSpline(Trail, ESplineCoordinateSpace::World), 20.f, 32, FColor::Green);
	}
	else if (Target != nullptr)	// Target Guided
	{
		FVector TargetGuideDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector ForwardDir = GetActorForwardVector();
		FVector LaunchDir = ForwardDir;
		float Rad = FMath::Acos(FVector::DotProduct(ForwardDir, TargetGuideDir));
		DynamicRotDegPerTick = FMath::Clamp(
			DynamicRotDegPerTick - DeltaTime * MaxRotDegPerTick * 4.f, // Angle Decrease
			MaxRotDegPerTick, DynamicRotDegPerTick); // Clamp

		if (FMath::Abs(FMath::RadiansToDegrees(Rad)) <= DynamicRotDegPerTick)
		{
			LaunchDir = TargetGuideDir;
		}
		else
		{
			FVector RotAxis = FVector::CrossProduct(ForwardDir, TargetGuideDir).GetSafeNormal();
			LaunchDir = ForwardDir.RotateAngleAxis(DynamicRotDegPerTick, RotAxis);
		}
		//PostLocation = GetActorLocation() + LaunchDir * Vel * DeltaTime;
		Dir = LaunchDir;

	}
	else // Straight
	{
		//PostLocation = GetActorLocation() + Dir * Vel * DeltaTime;
	}
	PostLocation = GetActorLocation() + Dir * Vel * DeltaTime;

	TArray<FHitResult> HitResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(6.f);
	GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), PostLocation, FQuat(), ECollisionChannel::ECC_Pawn, CollisionShape, CollisionQueryParam);
	SetActorLocation(PostLocation);
	SetActorRotation(Dir.Rotation());

	for (const FHitResult& HitResult : HitResults)
	{
		//if (HitResult.GetActor() != nullptr) UE_LOG(LogTemp, Log, TEXT("Projectile : Hit Actor : %s"), *HitResult.GetActor()->GetName());		
		//if (HitResult.GetComponent() != nullptr) UE_LOG(LogTemp, Log, TEXT("Projectile : Hit Component : %s"), *HitResult.GetComponent()->GetName());
		IIDamagable* DMG = Cast<IIDamagable>(HitResult.GetActor());
		if (HitResult.GetComponent()->IsA<UStaticMeshComponent>())
		{
			Trail = 0.f;
			if (ObjectPoolManager != nullptr) ObjectPoolManager->ReturnProjectile(this);
			SetActorTickEnabled(false);
			if (ParticleSystemComponent != nullptr) ParticleSystemComponent->Deactivate();
			ParticleSystemComponent = nullptr;
		}
		else if (DMG != nullptr && HitResult.GetActor() != Config.Causer)
		{
			if (HitResult.GetActor()->Implements<UIEnemyCharacter>() == Config.Causer->Implements<UIEnemyCharacter>()) continue;
			Config.HitLocation = HitResult.ImpactPoint;
			Config.HitDirection = Dir;
			Config.CausedTimeSeconds = GetWorld()->TimeSeconds;
			Config.AttackType = ATTACK_TYPE_RANGED;
			Config.DamageWidgetColor = DAMAGE_COLOR_MINION;
			if (DMG->RecieveDamage(Config))
			{
				Trail = 0.f;
				if (ObjectPoolManager != nullptr) ObjectPoolManager->ReturnProjectile(this);
				SetActorTickEnabled(false);
				if (ParticleSystemComponent != nullptr) ParticleSystemComponent->Deactivate();
				ParticleSystemComponent = nullptr;
			}
		}
	}
	Trail += Vel * DeltaTime;

	if (Trail >= MaxRange)
	{
		SetActorTickEnabled(false);
		if (ParticleSystemComponent != nullptr) ParticleSystemComponent->Deactivate();
		ParticleSystemComponent = nullptr;
		Trail = 0.f;
		if (Config.ProjectileExpiredEffect != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Config.ProjectileExpiredEffect, GetActorTransform(), true, EPSCPoolMethod::AutoRelease);
	}
}

