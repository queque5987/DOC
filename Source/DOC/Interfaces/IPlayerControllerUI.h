#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "IPlayerControllerUI.generated.h"

UINTERFACE(MinimalAPI)
class UIPlayerControllerUI : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIPlayerControllerUI
{
	GENERATED_BODY()

public:
	virtual class IIHUD* GetHUDInterface() { return nullptr; };
	virtual class IIHUD* GetWidemapInterface() { return nullptr; };
	virtual void ToggleInventory() {};
	//virtual bool InsertItem(FINSERT_ITEM*& Delegate_InsertItem, class AActor* Item, int32 ItemType) { return bool(); };
	//virtual bool InsertItem(class UCItemData* ItemData, class AActor* Item, int32 ItemCategory, int32 ItemType) { return bool(); };
	virtual bool InsertItem(class UCItemData* ItemData, class AActor* Item) { return bool(); };
	virtual bool InsertEquipment(class UCItemData* ItemData, class AActor* Equipment, int32 EquipmentType) { return bool(); };
	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) {};
	virtual void ToggleWidemap(bool e) {};
	virtual void ToggleMinimap(bool e) {};
	virtual void ToggleBossHPBar(bool e, class IIDamagable* InBoss) {};
	virtual bool IsInventoryVisible() { return false; };
	virtual void ShowItemTooltip(class UCItemData* ItemData) {};
	virtual void HideItemTooltip() {};
	virtual void EquipItem(class UCItemData* ItemData) {};
	virtual void UnEquipItem(class UCItemData* ItemData) {};
	virtual bool GetHasWeapon() { return false; };
	virtual void SetHasWeapon(bool bHasWeapon) {};
	virtual void OpenChestItemWidget(TArray<class UCItemData*>& ToShowItemData) {};
	virtual void CloseChestItemWidget() {};
};
