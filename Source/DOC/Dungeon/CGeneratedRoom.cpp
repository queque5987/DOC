#include "CGeneratedRoom.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IInteractableItem.h"

ACGeneratedRoom::ACGeneratedRoom()
{
	PrimaryActorTick.bCanEverTick = false;
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Room Enter Collider"));
	Collider->SetupAttachment(GetRootComponent());
	Collider->SetRelativeScale3D(FVector(4.f, 4.f, 4.f));
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

//void ACGeneratedRoom::Generate(int32 GridScale, float StageScale)
//{
//	for (int32 i = 0; i < GridScale; i++)
//	{
//		for (int32 j = 0; j < GridScale; j++)
//		{
//			HISM_Floor1->AddInstance(
//				FTransform(
//					FRotator(0.f, 0.f, 0.f),
//					FVector(
//						i * 100.f * StageScale,
//						j * 100.f * StageScale,
//						0.f
//					)
//				)
//			);
//
//			if (i == 0)
//			{
//				HISM_Wall->AddInstance(
//					FTransform(
//						FRotator(0.f, 90.f, 0.f),
//						FVector(
//							i * 100.f * StageScale - 100.f * StageScale / 2.f,
//							j * 100.f * StageScale,
//							0.f
//						)
//					)
//				);
//				HISM_Wall_Base->AddInstance(
//					FTransform(
//						FRotator(0.f, 90.f, 0.f),
//						FVector(
//							i * 100.f * StageScale - 100.f * StageScale / 2.f,
//							j * 100.f * StageScale,
//							0.f
//						)
//					)
//				);
//			}
//			else if (i == GridScale - 1)
//			{
//				HISM_Wall->AddInstance(
//					FTransform(
//						FRotator(0.f, 90.f, 0.f),
//						FVector(
//							i * 100.f * StageScale + 100.f * StageScale / 2.f,
//							j * 100.f * StageScale,
//							0.f
//						)
//					)
//				);
//				HISM_Wall_Base->AddInstance(
//					FTransform(
//						FRotator(0.f, 90.f, 0.f),
//						FVector(
//							i * 100.f * StageScale + 100.f * StageScale / 2.f,
//							j * 100.f * StageScale,
//							0.f
//						)
//					)
//				);
//			}
//			if (j == 0)
//			{
//				HISM_Wall->AddInstance(
//					FTransform(
//						FRotator(0.f, 0.f, 0.f),
//						FVector(
//							i * 100.f * StageScale,
//							j * 100.f * StageScale - 100.f * StageScale / 2.f,
//							0.f
//						)
//					)
//				);
//				HISM_Wall_Base->AddInstance(
//					FTransform(
//						FRotator(0.f, 0.f, 0.f),
//						FVector(
//							i * 100.f * StageScale,
//							j * 100.f * StageScale - 100.f * StageScale / 2.f,
//							0.f
//						)
//					)
//				);
//			}
//			else if (j == GridScale - 1)
//			{
//				HISM_Wall->AddInstance(
//					FTransform(
//						FRotator(0.f, 0.f, 0.f),
//						FVector(
//							i * 100.f * StageScale,
//							j * 100.f * StageScale + 100.f * StageScale / 2.f,
//							0.f
//						)
//					)
//				);
//				HISM_Wall_Base->AddInstance(
//					FTransform(
//						FRotator(0.f, 0.f, 0.f),
//						FVector(
//							i * 100.f * StageScale,
//							j * 100.f * StageScale + 100.f * StageScale / 2.f,
//							0.f
//						)
//					)
//				);
//			}
//		}
//	}
//}

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
	}
	else UE_LOG(LogTemp, Log, TEXT("ACGeneratedRoom : OnPlayerEnteredRoom : PlacedDoor nullptr"));
}


