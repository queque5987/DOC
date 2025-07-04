#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "IStageGrid_Meshes.generated.h"

UINTERFACE(MinimalAPI)
class UIStageGrid_Meshes : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIStageGrid_Meshes
{
	GENERATED_BODY()
public:
	virtual void AddGrid(FTransform Transform, int32 Type, bool bWorldSpace = false) {};
	virtual void ClearGrid() {};
	virtual void GenerateStair(bool bIsEntrance, FLIGHTS_ON* DeleLightOn, FENTERED_STAGE* DeleEnteredStage) {};
	virtual void TurnOnLights(class IIObjectPoolManager* LightManager) {};
	virtual void TurnOffLights(class IIObjectPoolManager* LightManager) {};
	//virtual bool GetHasDoor() { return bool(); };
	//virtual void SetHasDoor(bool b) {};
	//virtual void SetRoomRelativeLocation(FVector Location) {};
};
