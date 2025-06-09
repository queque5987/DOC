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
};
