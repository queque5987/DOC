#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IObjectPoolManager.generated.h"

UINTERFACE(MinimalAPI)
class UIObjectPoolManager : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIObjectPoolManager
{
	GENERATED_BODY()

public:
	//virtual class UPointLightComponent* GetPointLightComponent(class AActor* OwningActor) { return nullptr; };
	//virtual void ReturnPointLightComponent(class UPointLightComponent* PLC) {};
	virtual class UStaticMeshComponent* GetStaticMeshComponent(class AActor* OwningActor, int32 Type, FTransform Transform) { return nullptr; };
	virtual void ReturnStaticMeshComponent(class UStaticMeshComponent* PLC, int32 Type) {};
	virtual class IIInteractableItem* GetChest(class AActor* OwningActor, FTransform Transform) { return nullptr; };
	virtual void ReturnChest(class IIInteractableItem* Chest) {};
	virtual class IIInteractableItem* GetItem_InChest(class AActor* OwningActor, int32 Type, FTransform Transform) { return nullptr; };
	virtual void ReturnItem(class IIInteractableItem* Item, int32 Type) {};
	virtual void ReturnItem(class AActor* Item, int32 Type) {};
	virtual class IIInteractableItem* GetDoor(class AActor* OwningActor, FTransform Transform) { return nullptr; };
	virtual void ReturnDoor(class IIInteractableItem* Door) {};
	virtual class IIInteractableItem* GetBrazier(class AActor* OwningActor, FTransform Transform) { return nullptr; };
	virtual void ReturnBrazier(class IIInteractableItem* Brazier) {};

	virtual void RebuildNavMesh() {};
	virtual void SetNavMeshLocation(FTransform& NewLocation) {};
	virtual void GenerateNextStage() {};
};
