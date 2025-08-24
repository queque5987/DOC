#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/IGameStateLightManager.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/INavSystemManager.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "PCH.h"
#include "CGameState_Stage.generated.h"

UCLASS()
class DOC_API ACGameState_Stage : public AGameStateBase, public IIGameStateLightManager, public IIObjectPoolManager, public IINavSystemManager
{
	GENERATED_BODY()

	ACGameState_Stage();
	virtual void PostInitializeComponents() override;

protected:
	TArray<class ACGeneratedStage*> Stages;

	TArray<class UPointLightComponent*> PointLights;
	TQueue<class UPointLightComponent*> PointLights_Available;

	TArray<class UStaticMesh*> StaticMeshes;
	TArray<TArray<class UStaticMeshComponent*>> StaticMeshComponents;
	TArray<TQueue<class UStaticMeshComponent*>> StaticMeshes_Available;

	TArray<class ACChest*> Chests;
	TQueue<class ACChest*> Chests_Available;

	TArray<class ACDoor*> Doors;
	TQueue<class ACDoor*> Doors_Available;

	TArray<class ACBrazier*> Braziers;
	TQueue<class ACBrazier*> Braziers_Available;

	TArray<TSubclassOf<AActor>> ItemClasses;
	TArray<TArray<AActor*>> Items;
	TArray<TQueue<AActor*>> Item_Available;

	TArray<TSubclassOf<AActor>> EnemyClasses;
	TArray<TArray<AActor*>> Enemies;
	TArray<TQueue<AActor*>> Enemy_Available;

	TArray<class UParticleSystem*> ParticleSystems;

	TArray<class ACProjectile*> Projectiles;
	TQueue<class ACProjectile*> Projectiles_Available;

	TArray<class UCDamage*> DamageComponents;
	TQueue<class UCDamage*> DamageComponents_Available;

	TMap<class UStaticMesh*, TArray<class UMaterialInterface*>> StaticMeshTranslucentMaterialInstance;
	TMap<class UStaticMesh*, TArray<class UMaterialInterface*>> StaticMeshOpaqueMaterialInstance;

	TArray<TSubclassOf<AActor>> EquipmentsClasses;
	TArray<TArray<AActor*>> Equipments;
	TArray<TQueue<AActor*>> Equipments_Available;

	class ANavMeshBoundsVolume* NavVolume;
	class UNavigationSystemV1* NavSystem;
	class UCSubsystem_ItemManager* ItemManager;

	int32 StairCoord_x = 2;
	int32 StairCoord_y = 2;
	int32 StairCoord_d = 0;

	FStageCleared* StageClearedDelegatePtr;
	//class IIGameModeDataManager* GameModeDataManager;
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void HandleBeginPlay() override;
	virtual void BeginPlay() override;

// Object Pooiing

	virtual class UPointLightComponent* GetPointLightComponent(class AActor* OwningActor) override;
	virtual void ReturnPointLightComponent(class UPointLightComponent* PLC) override;

	virtual class UStaticMeshComponent* GetStaticMeshComponent(class AActor* OwningActor, int32 Type, FTransform Transform) override;
	virtual void ReturnStaticMeshComponent(class UStaticMeshComponent* SMC, int32 Type) override;

	virtual class IIInteractableItem* GetChest(class AActor* OwningActor, FTransform Transform) override;
	virtual void ReturnChest(class IIInteractableItem* Chest) override;

	virtual class IIInteractableItem* GetItem_InChest(class AActor* OwningActor, int32 Type, FTransform Transform) override;
	virtual void ReturnItem(class IIInteractableItem* Item, int32 Type) override;
	virtual void ReturnItem(class AActor* Item, int32 Type) override;

	virtual class IIInteractableItem* GetDoor(class AActor* OwningActor, FTransform Transform) override;
	virtual void ReturnDoor(class IIInteractableItem* Door) override;

	virtual class IIInteractableItem* GetBrazier(class AActor* OwningActor, FTransform Transform) override;
	virtual void ReturnBrazier(class IIInteractableItem* Brazier) override;

	virtual class IIEnemyCharacter* GetEnemyCharacter(class AActor* OwningActor, int32 Type, FTransform Transform) override;
	virtual void ReturnEnemyCharacter(class IIEnemyCharacter* EnemyCharacter, int32 Type) override;

	virtual class IIEquipment* GetEquipment(class AActor* OwningActor, int32 Type, FTransform Transform) override;
	virtual void ReturnEquipment(class IIEquipment* Equipment, int32 Type) override;

	virtual class UParticleSystemComponent* SpawnParticle(class USceneComponent* AttachComponent, FName AttachPointName, int32 Type, FTransform Transform) override;
	virtual void SpawnProjectile(FTransform Transform, FDamageConfig DamageConfig, class AActor* TargetActor, float Velocity, int32 ProjectileParticleType) override;
	virtual void ReturnProjectile(class ACProjectile* Projectile) override;

	virtual class UCDamage* GetDamageComponent(class AActor* OwningActor, FDamageConfig DamageConfig) override;
	virtual void ReturnDamageComponent(class UCDamage* DamageComponent) override;

	virtual void SetStaticMeshLOD(class UStaticMeshComponent* StaticMeshComp, int32 LODs, bool IsNanite = true) override;
	virtual class UCItemData* GetItemData(int32 ItemCategory, int32 ItemCode, int32 ItemCount) override;

// NavSystem
	virtual void SetNavigationInvoker(class AActor* Character) override;
	virtual void RebuildNavMesh() override;
	virtual void SetNavMeshLocation(FTransform& NewLocation) override;
	virtual FVector GetRandomNavigatablePoint_ExclusiveRadius(FVector CurrentPosition, float MinDistance, float MaxDistance, FVector ExclusivePosition, float ExclusiveRadius, int32 Trial = 4) override;
	virtual bool GetRandomNagivatablePoint_AwayFromObject(FVector OirignPos, FVector AwayPos, float Distance, float Tolerance, FVector& OutPos) override;
	virtual void GenerateNextStage() override;

	virtual void GetTranslucentMaterialInstance(class UStaticMesh* StaticMesh, TArray<class UMaterialInterface*>& OutArray) override;
	virtual void GetOpaqueMaterialInstance(class UStaticMesh* StaticMesh, TArray<class UMaterialInterface*>& OutArray) override;
};
