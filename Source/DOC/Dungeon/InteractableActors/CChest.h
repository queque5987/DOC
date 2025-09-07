#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IInteractableChest.h"
#include "PCH.h"
#include "CChest.generated.h"

UCLASS()
class DOC_API ACChest : public AActor, public IIInteractableItem
{
	GENERATED_BODY()
	
public:	
	ACChest();

protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SM_Chest_Top;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SM_Chest_Body;
	UPROPERTY(VisibleAnywhere)
	class URectLightComponent* RectLight;

	float OpenSpeed = 1.2f;
	bool bBusy = false;
	bool Selected = false;
	float MaxAngle = 67.f;
	float ToggleAngle = 0.f;
	float Angle;
	TTuple<int32, int32, int32> PlacedCoordinate;
	class IIGeneratedStage* PlacedStage;
	TArray<class IIInteractableItem*> SpawnedItems;
	virtual void BeginPlay() override;

	TMap<int32, FVector> SpawnItemScales;

	TArray<class UCItemData*> PossessItems;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) override;
	virtual void ClearItemData() override;
	virtual void AddItemData(class UCItemData* InItemData) override;
	//virtual bool GetBusy() override { return bBusy; };
	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override;
	virtual void Select() override;
	virtual void UnSelect() override;
	virtual void SetVisibility(bool e) override;
	virtual bool GetVisibility() override;
	
	virtual IIInteractableItem* SpawnItemToStage(int32 ItemType, class IIObjectPoolManager* ObjectPoolManager) override;
	virtual IIInteractableItem* SpawnEquipmentToStage(int32 EquipmentType, class IIObjectPoolManager* ObjectPoolManager) override;
	virtual void ReturnItemsFromStage(class IIObjectPoolManager* ObjectPoolManager, TArray<TArray<int32>>& SpawnItems) override;
	virtual void ManualInteract(int32 OpenState, bool bForce = true) override;
	virtual void SetPlacedCoordinate(TTuple<int32, int32, int32> Coordinate, class IIGeneratedStage* Stage, int32 idx = 0) override {
		PlacedCoordinate = Coordinate; 
		PlacedStage = Stage;
	};
	virtual TTuple<int32, int32, int32> GetPlacedCoordinate() override { return PlacedCoordinate; };
protected:
	void ToggleBox();
};
