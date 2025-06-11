#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPlayerControllerStage.generated.h"

UINTERFACE(MinimalAPI)
class UIPlayerControllerStage : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIPlayerControllerStage
{
	GENERATED_BODY()

public:
	virtual void MinimapRemoveBind() {};
	virtual void LightsOutPrevStage() {};
	virtual void SetStage(class IIGeneratedStage* Stage) {};
	virtual void SightReached(FVector& SightLocation) {};
	virtual FVector GetPlayerLocation() { return FVector(); };
};
