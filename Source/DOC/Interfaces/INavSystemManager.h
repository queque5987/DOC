#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "INavSystemManager.generated.h"

UINTERFACE(MinimalAPI)
class UINavSystemManager : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IINavSystemManager
{
	GENERATED_BODY()

public:
	// TODO Move
	//virtual void RebuildNavMesh() {};
	//virtual void SetNavMeshLocation(FTransform& NewLocation) {};
	virtual FVector GetRandomNavigatablePoint_ExclusiveRadius(FVector CurrentPosition, float MinDistance, float MaxDistance, FVector ExclusivePosition, float ExclusiveRadius, int32 Trial = 10) { return FVector(); }
	virtual bool GetRandomNagivatablePoint_AwayFromObject(FVector OirignPos, FVector AwayPos, float Distance, float Tolerance, FVector& OutPos) { return bool(); };
};
