#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameStateLightManager.generated.h"

UINTERFACE(MinimalAPI)
class UIGameStateLightManager : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIGameStateLightManager
{
	GENERATED_BODY()

public:
	//virtual class UPointLightComponent* GetPointLightComponent(class AActor* OwningActor) { return nullptr; };
	//virtual void ReturnPointLightComponent(class UPointLightComponent* PLC) {};
};
