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
	virtual void SpawnBrazier() {};
	virtual void SpawnChest() {};
	virtual bool GetHasDoor() { return bool(); };
	virtual void SetHasDoor(bool b) {};
	virtual void SetDoorPtr(class IIInteractableItem* Door) {};
	virtual void SetObjectPoolManager(class IIObjectPoolManager* IOPM) {};
	virtual void SetRoomRelativeLocation(FVector Location) {};
	virtual void SetDoorLocation(FVector Location) {};
	virtual void TurnOnLights(class IIGameStateLightManager* LightManager) {};
	virtual void TurnOffLights(class IIGameStateLightManager* LightManager) {};
};
