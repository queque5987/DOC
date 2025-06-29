#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageStructs.h"
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
	virtual class UPointLightComponent* GetPointLightComponent(class AActor* OwningActor) { return nullptr; };
	virtual void ReturnPointLightComponent(class UPointLightComponent* PLC) {};
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
	virtual class IIEnemyCharacter* GetEnemyCharacter(class AActor* OwningActor, int32 Type, FTransform Transform) { return nullptr; };
	virtual void ReturnEnemyCharacter(class IIEnemyCharacter* EnemyCharacter, int32 Type) {};

	virtual class UParticleSystemComponent* SpawnParticle(class USceneComponent* AttachComponent, FName AttachPointName, int32 Type, FTransform Transform) { return nullptr; };
	virtual void SpawnProjectile(FTransform Transform, FDamageConfig DamageConfig, class AActor* TargetActor, float Velocity, int32 ProjectileParticleType) {};
	virtual void ReturnProjectile(class ACProjectile* Projectile) {};

	virtual void RebuildNavMesh() {};
	virtual void SetNavMeshLocation(FTransform& NewLocation) {};
	virtual void GenerateNextStage() {};

	virtual void GetTranslucentMaterialInstance(class UStaticMesh* StaticMesh, TArray<class UMaterialInterface*>& OutArray) {};
	virtual void GetOpaqueMaterialInstance(class UStaticMesh* StaticMesh, TArray<class UMaterialInterface*>& OutArray) {};

	virtual void SetStaticMeshLOD(class UStaticMeshComponent* StaticMeshComp, int32 LODs, bool IsNanite = true) {};
};
