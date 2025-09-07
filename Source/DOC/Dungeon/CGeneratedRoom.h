#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PCH.h"
#include "Dungeon/CStageGrid_Corridor.h"
#include "Interfaces/IStageGrid_Room.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CGeneratedRoom.generated.h"


UCLASS()
class DOC_API ACGeneratedRoom : public ACStageGrid_Corridor, public IIStageGrid_Room
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	class UBoxComponent* Collider;
public:	
	ACGeneratedRoom();

protected:
	class IIInteractableItem* PlacedDoor;
	class IIObjectPoolManager* ObjectPoolManager;
	virtual void BeginPlay() override;
	bool bHasDoor = false;
	FVector2D Size;
	TArray<int32> ToSpawnEnemies;
	UPROPERTY(VisibleAnywhere, Category = "Room")
	TArray<class UCSpawnedEnemyData*> SpawnedEnemies;
	TArray<class UCItemData*> ClearBonusItemsArr;
	FStageCleared* StageClearedDelegatePtr;
	bool IsLocationInRoom(FVector Location);
	class IIPlayerOnStage* EnteredCharacter;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetRoomSize(FVector2D RoomSize) override { Size = RoomSize; };
	virtual bool GetHasDoor() override { return bHasDoor; };
	virtual void SetHasDoor(bool b) override { bHasDoor = b; };
	virtual void SetDoorPtr(class IIInteractableItem* Door) override { PlacedDoor = Door; };
	virtual void SetObjectPoolManager(class IIObjectPoolManager* IOPM) override { ObjectPoolManager = IOPM; };
	virtual void SetRoomRelativeLocation(FVector Location) override { SetActorRelativeLocation(Location); };
	virtual void SetDoorLocation(FVector Location) override;
	virtual bool GetRangedAttackPosition(FVector Origin, FVector Target, float Range, float MaxAngle, FVector& OutVector) override;
	virtual void AddSpawnEnemy(int32 EnemyType) override { ToSpawnEnemies.Add(EnemyType); };
	virtual void SetClearedDelegatePtr(FStageCleared* InStageClearedDelegate) override;
	UFUNCTION()
	void OnPlayerEnteredRoom(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSpawnedEnemyDiedCompleted(FDamageConfig DamageConfig);
};
