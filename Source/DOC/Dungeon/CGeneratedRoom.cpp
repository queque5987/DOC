#include "CGeneratedRoom.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IEnemyCharacter.h"
#include "DrawDebugHelpers.h"

ACGeneratedRoom::ACGeneratedRoom()
{
	PrimaryActorTick.bCanEverTick = false;
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Room Enter Collider"));
	Collider->SetupAttachment(GetRootComponent());
	Collider->SetRelativeScale3D(FVector(3.f, 3.f, 3.f));
}

void ACGeneratedRoom::BeginPlay()
{
	Super::BeginPlay();
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ACGeneratedRoom::OnPlayerEnteredRoom);
	Collider->SetCollisionResponseToChannel(COLLISION_CHANNEL_PLAYER_GAZE, ECollisionResponse::ECR_Ignore);
}

bool ACGeneratedRoom::IsLocationInRoom(FVector Location)
{
	float MX = GetActorLocation().X + Size.X * 0.9f / 2.f;
	float mx = GetActorLocation().X - Size.X * 0.9f / 2.f;
	float MY = GetActorLocation().Y + Size.Y * 0.9f / 2.f;
	float my = GetActorLocation().Y - Size.Y * 0.9f / 2.f;

	return ((Location.X < mx || Location.X > MX) || (Location.Y < my || Location.Y > MY) ? false : true);
}

void ACGeneratedRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACGeneratedRoom::SetDoorLocation(FVector Location)
{
	Collider->SetWorldLocation(Location + FVector(0.f, 0.f, 100.f));
}

bool ACGeneratedRoom::GetRangedAttackPosition(FVector Origin, FVector Target, float Range, float MaxAngle, FVector& OutVector)
{
	float Distance = FVector::Dist2D(Origin, Target);
	float Angle = FMath::Abs(Distance - Range) * MaxAngle;

	FVector Direction = (Target - Origin).GetSafeNormal2D();
	FVector TargetToOrigin = Origin - Target;
	/*
	AttackPos = Origin + Direction * t;
	||AttackPos - Target||^2= Range^2;
	||Origin + Direction * t - Target||^2= Range^2;
	m = Origin - Target
	||Origin - Target + Direction * t||^2= Range^2;
	||m + Direction * t||^2= Range^2;
	(m + Direction * t) * (m + Direction * t)= Range^2;
	Dt^2 + 2mDt + m^2 - Range^2 = 0;
	A = D * D
	B = 2 * m * D
	C = m * m - Range^2
	t =  (-B +- sqrt(B^2 - 4AC)) / 2A
	*/

	float A = FVector::DotProduct(Direction, Direction);
	float B = FVector::DotProduct(TargetToOrigin, Direction) * 2.f;
	float C = FVector::DotProduct(TargetToOrigin, TargetToOrigin) - Range * Range;

	float tm = (-B + FMath::Sqrt(B * B - 4.f * A * C)) / (2.f * A);
	float tp = (-B - FMath::Sqrt(B * B - 4.f * A * C)) / (2.f * A);
	
	FVector ReDirection = FRotator(0.f, FMath::FRandRange(-MaxAngle, MaxAngle), 0.f).RotateVector(Direction);
	FVector TowardPos = Origin + Direction * tp;
	FVector TargetToTowardDirection = TowardPos - Target;

	float R_Angle = FMath::FRandRange(-MaxAngle, MaxAngle);
	FVector a = Target + FRotator(0.f, R_Angle, 0.f).RotateVector(TargetToTowardDirection);
	FVector b = Target + FRotator(0.f, R_Angle + 90.f, 0.f).RotateVector(TargetToTowardDirection);
	FVector c = Target + FRotator(0.f, R_Angle + 180.f, 0.f).RotateVector(TargetToTowardDirection);
	FVector d = Target + FRotator(0.f, R_Angle - 90.f, 0.f).RotateVector(TargetToTowardDirection);

	//FVector a = Origin + ReDirection * tm;
	//FVector b = Origin + ReDirection * tp;
	//FVector c = Origin + FRotator(0.f, 90.f, 0.f).RotateVector(ReDirection) * tp;
	//FVector d = Origin + FRotator(0.f, -90.f, 0.f).RotateVector(ReDirection) * tp;

	//DrawDebugSphere(GetWorld(), a, 50.f, 32, FColor::White, false, 2.f);
	//DrawDebugSphere(GetWorld(), b, 50.f, 32, FColor::White, false, 2.f);
	//DrawDebugSphere(GetWorld(), c, 50.f, 32, FColor::White, false, 2.f);
	//DrawDebugSphere(GetWorld(), d, 50.f, 32, FColor::White, false, 2.f);

	//if (IsLocationInRoom(b))
	//{
	//	OutVector = b;
	//	DrawDebugSphere(GetWorld(), OutVector, 50.f, 32, FColor::Green, false, 2.f);
	//	return true;
	//}
	//if (IsLocationInRoom(c))
	//{
	//	OutVector = c;
	//	DrawDebugSphere(GetWorld(), OutVector, 50.f, 32, FColor::Green, false, 2.f);
	//	return true;
	//}
	//if (IsLocationInRoom(d))
	//{
	//	OutVector = d;
	//	DrawDebugSphere(GetWorld(), OutVector, 50.f, 32, FColor::Green, false, 2.f);
	//	return true;
	//}
	//if (IsLocationInRoom(a))
	//{
	//	OutVector = a;
	//	DrawDebugSphere(GetWorld(), OutVector, 50.f, 32, FColor::Green, false, 2.f);
	//	return true;
	//}
	return false;
}

void ACGeneratedRoom::OnPlayerEnteredRoom(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IIPlayerOnStage* PlayerCharacter = Cast<IIPlayerOnStage>(OtherActor);
	if (PlayerCharacter == nullptr) return;

	if (PlacedDoor != nullptr)
	{
		PlacedDoor->ManualInteract(INTERACTABLE_ITEM_STATE_CLOSED, false);
		PlacedDoor->SetLocked(true);
		
		if (ObjectPoolManager != nullptr)
		{
			for(int32 SpawnEnemyType : ToSpawnEnemies)
			{
				IIEnemyCharacter* EC = ObjectPoolManager->GetEnemyCharacter(this, ENEMYCHARACTER_MINION, GetActorTransform());
				if (EC != nullptr)
				{
					EC->SetEnemyType(SpawnEnemyType);
					EC->SetSpawnedRoom(this);
					EC->SetObjectPoolManager(ObjectPoolManager);
					FVector SpawnLocation = 
						GetActorLocation() +
						FVector(
							FMath::RandRange(-1.f, 1.f) * Size.X / 6.f,
							FMath::RandRange(-1.f, 1.f) * Size.Y / 6.f,
							50.f
						);
					FRotator SpawnRotation = (GetActorLocation() - SpawnLocation).GetSafeNormal2D().Rotation();
					FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation, FVector(1.f, 1.f, 1.f));
					Cast<AActor>(EC)->SetActorTransform(SpawnTransform);
					ObjectPoolManager->SpawnParticle(EC->GetSKMesh(), NAME_None, PARTICLE_MINION_SPAWN, SpawnTransform);
					FOnDeath* tempDelegate = EC->GetOnDiedCompletedDelegate();
					if (tempDelegate != nullptr)
					{
						FEnemyInfo tempEnemyInfo{ EC, tempDelegate, tempDelegate->AddUFunction(this, TEXT("OnSpawnedEnemyDiedCompleted")) };
						SpawnedEnemies.Add(tempEnemyInfo);
					}
					else UE_LOG(LogTemp, Error, TEXT("While Spawning Minion No Delegate Found"));
				}
			}
		}
		else UE_LOG(LogTemp, Log, TEXT("ACGeneratedRoom : OnPlayerEnteredRoom : ObjectPoolManager nullptr"));
	}
	else UE_LOG(LogTemp, Log, TEXT("ACGeneratedRoom : OnPlayerEnteredRoom : PlacedDoor nullptr"));

	Collider->OnComponentBeginOverlap.RemoveDynamic(this, &ACGeneratedRoom::OnPlayerEnteredRoom);
}

void ACGeneratedRoom::OnSpawnedEnemyDiedCompleted(FDamageConfig DamageConfig)
{
	if (DamageConfig.Causer != nullptr)
	{
		IIEnemyCharacter* IEC = Cast<IIEnemyCharacter>(DamageConfig.Causer);
		if (IEC != nullptr)
		{
			bool IsAnySurviver = false;
			for (FEnemyInfo& EInfo : SpawnedEnemies)
			{
				if (EInfo.Enemy == IEC)
				{
					EInfo.bDead = true;
					EInfo.OnDiedCompletedDelegate->Remove(EInfo.OnDiedCompletedHandle);
					ObjectPoolManager->ReturnEnemyCharacter(IEC, IEC->GetEnemyType());
				}
				if (!IsAnySurviver && !EInfo.bDead) IsAnySurviver = true;
			}

			if (!IsAnySurviver)
			{
				if (PlacedDoor != nullptr)
				{
					PlacedDoor->ManualInteract(INTERACTABLE_ITEM_STATE_CLOSED, false);
					PlacedDoor->SetLocked(false);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ACGeneratedRoom::OnSpawnedEnemyDiedCompleted : DamageConfig.Causer is nullptr"));
	}
}


