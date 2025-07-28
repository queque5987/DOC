#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "IPlayerState.generated.h"

UINTERFACE(MinimalAPI)
class UIPlayerState : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIPlayerState
{
	GENERATED_BODY()

public:
	virtual void AddInventoryItem(class UDataAsset* ItemData) {};
	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) {};
	virtual bool InsertItem(class UCItemData* ItemData, class UCItemData*& RtnItemData) { return bool(); };
	virtual void SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem) {};
	virtual void SetEquipDelegates(FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipDelegate) {};
	virtual FHP_CHANGED* GetHPChangedDelegate() { return nullptr; };
	virtual FOnStatusChanged* GetOnStatusChangedDelegate() { return nullptr; };
	virtual FOnPlayerInventoryChanged* GetOnInventoryChangedDelegate() { return nullptr; };
	virtual void RecieveDamage(float DamageAmount) {};
	virtual float GetHP() { return 0.f; };
	virtual float GetMaxHP() { return 0.f; };
	virtual void SetHP(float e) {};
	virtual void SetMaxHP(float e) {};
	virtual void RemoveItem(class UCItemData* ItemData) {};
	virtual bool GetHasWeapon() { return false; };
	virtual void SetHasWeapon(bool bHasWeapon) {};
	virtual void RecalculateTotalStats() {};
	virtual class UCItemData* GetEquippedItemData(int32 ItemEquipSlot) { return nullptr; };
	virtual FPlayerStat GetPlayerStat() { return FPlayerStat(); };
};
