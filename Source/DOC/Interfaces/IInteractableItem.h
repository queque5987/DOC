#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractableItem.generated.h"

UINTERFACE(MinimalAPI)
class UIInteractableItem : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIInteractableItem
{
	GENERATED_BODY()

public:
	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) { return bool(); };
	virtual void Select() {};
	virtual void UnSelect() {};
	virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) {};
	//virtual bool GetBusy() { return bool(); };
	virtual void SetItemType(int32 Type) {};
	virtual int32 GetItemType() { return int32(); };
	virtual const int32 GetItemCategory() { return int32(); };
	virtual void SetVisibility(bool e) {};
	virtual bool GetVisibility() { return bool(); };
	virtual void SetItemData(class UCItemData* ItemDataAsset) {};
	virtual void ClearItemData() {};
	virtual void AddItemData(class UCItemData* InItemData) {};
	virtual IIInteractableItem* SpawnItemToStage(int32 ItemType, class IIObjectPoolManager* ObjectPoolManager) { return nullptr; };
	virtual IIInteractableItem* SpawnEquipmentToStage(int32 EquipmentType, class IIObjectPoolManager* ObjectPoolManager) { return nullptr; };
	virtual void ReturnItemsFromStage(class IIObjectPoolManager* ObjectPoolManager, TArray<TArray<int32>>& SpawnItems) {};
	virtual void ManualInteract(int32 OpenState, bool bForce = true) {};
	virtual void SetLocked(bool e) {};
	virtual void SetPlacedCoordinate(TTuple<int32, int32, int32> Coordinate, class IIGeneratedStage* Stage, int32 idx = 0) {};
	virtual TTuple<int32, int32, int32> GetPlacedCoordinate() { return TTuple<int32, int32, int32>(); };
	virtual void SetChestSection(TArray<class IIInteractableItem*>* ChestItems, int32 idx) {};
};
