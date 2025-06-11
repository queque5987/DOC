#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGeneratedStage.generated.h"

UINTERFACE(MinimalAPI)
class UIGeneratedStage : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIGeneratedStage
{
	GENERATED_BODY()

public:
	virtual void ClearStage() {};
	virtual void GenerateStage() {};
	virtual void SetStairCoord(int32 Coord_H, int32 Coord_W, int32 Direction, int32& rtn_Coord_H, int32& rtn_Coord_W, int32& rtn_Direction, bool bIsEntrance = false) {};
	virtual void Exited_Stage() {};
	virtual void MinimapRemoveBind() {};
	virtual void LightsOut() {};
	virtual void PlayerSightReached(FVector& SightLocation) {};
	virtual void Stage_GridGenerate() {};
	virtual void Stage_GridReturn() {};

	virtual void SetChestOpenState(TTuple<int32, int32, int32> Coordinate, int32 OpenState) {};
	virtual void SetDoorOpenState(TTuple<int32, int32, int32> Coordinate, int32 OpenState, int32 index) {};
};
