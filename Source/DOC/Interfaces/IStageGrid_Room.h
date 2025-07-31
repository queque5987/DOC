#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IStageGrid_Room.generated.h"

UINTERFACE(MinimalAPI)
class UIStageGrid_Room : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIStageGrid_Room
{
	GENERATED_BODY()

public:
	virtual void SetRoomSize(FVector2D RoomSize) {};
	virtual bool GetHasDoor() { return bool(); };
	virtual void SetHasDoor(bool b) {};
	virtual void SetDoorPtr(class IIInteractableItem* Door) {};
	virtual void SetObjectPoolManager(class IIObjectPoolManager* IOPM) {};
	virtual void SetRoomRelativeLocation(FVector Location) {};
	virtual void SetDoorLocation(FVector Location) {};
	virtual void TurnOnLights(class IIObjectPoolManager* LightManager) {};
	virtual void TurnOffLights(class IIObjectPoolManager* LightManager) {};
	virtual void AddSpawnEnemy(int32 EnemyType) {};
	virtual bool GetRangedAttackPosition(FVector Origin, FVector Target, float Range, float MaxAngle, FVector& OutVector) { return bool(); };
};
