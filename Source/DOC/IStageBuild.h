#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IStageBuild.generated.h"

UINTERFACE(MinimalAPI)
class UIStageBuild : public UInterface
{
	GENERATED_BODY()
};

struct Room_Info;

class DOC_API IIStageBuild
{
	GENERATED_BODY()

public:
	//virtual void RegisterCharacterToStage(class ACharacter* PlayerCharacter, int32 floor) {};
	//virtual void RegisterMinimapRenderTargetToStage(class UCanvasRenderTarget2D* MinimapRenderTarget, int32 floor) {};
	//virtual void Debug_GenerateMap() {};
	//virtual void SetStairCoord(int32 Coord_H, int32 Coord_W, int32 Direction, int32& rtn_Coord_H, int32& rtn_Coord_W, int32& rtn_Direction, bool bIsEntrance = false) {};
	//virtual void GenerateStage() {};
	//virtual void ClearStage() {};
protected:
	//virtual bool CalculateClosestCorridor(TArray<FVector>& Trail) { return false; };
	//virtual class UStaticMesh* SpawnWall(FVector Location, int32 Type, int32 Distance = 0) { return nullptr; };
};
