#include "CGeneratedRoom.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IEnemyCharacter.h"

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

void ACGeneratedRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACGeneratedRoom::SetDoorLocation(FVector Location)
{
	Collider->SetWorldLocation(Location + FVector(0.f, 0.f, 100.f));
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
			IIEnemyCharacter* EC = ObjectPoolManager->GetEnemyCharacter(this, ENEMYCHARACTER_MINION, GetActorTransform());
			if (EC != nullptr) ObjectPoolManager->SpawnParticle(EC->GetSKMesh(), NAME_None, PARTICLE_MINION_SPAWN, FTransform());
		}
		else UE_LOG(LogTemp, Log, TEXT("ACGeneratedRoom : OnPlayerEnteredRoom : ObjectPoolManager nullptr"));
	}
	else UE_LOG(LogTemp, Log, TEXT("ACGeneratedRoom : OnPlayerEnteredRoom : PlacedDoor nullptr"));
}


