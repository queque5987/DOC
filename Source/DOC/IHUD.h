#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IHUD.generated.h"

UINTERFACE(MinimalAPI)
class UIHUD : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIHUD
{
	GENERATED_BODY()

public:
	virtual class UCanvasRenderTarget2D* GetMinimapRT2D() { return nullptr; };
	virtual void SetMinimapAngle(float Angle) {};
	//virtual void SetCurrentHP
};
