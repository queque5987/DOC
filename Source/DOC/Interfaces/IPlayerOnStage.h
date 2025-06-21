#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPlayerOnStage.generated.h"

UINTERFACE(MinimalAPI)
class UIPlayerOnStage : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIPlayerOnStage
{
	GENERATED_BODY()

public:
	virtual class IIPlayerControllerStage* GetPlayerControllerStage() { return nullptr; };
	virtual FVector GetLocation() { return FVector(); };
	virtual class UObject* GetControllerAsObject() { return nullptr; };
	virtual void MinimapRemoveDynamic() {};
	virtual void SetToPerspectiveCamera(FTransform Transform) {};
	virtual void SetToFollowCamera() {};
	virtual void LockOnMonster(class IIEnemyCharacter* Enemy) {};
	virtual void LockFreeMonster() {};
};
