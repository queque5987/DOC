#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IPlayerState.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "PCH.h"
#include "CPlayerState.generated.h"

UCLASS()
class DOC_API ACPlayerState : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()
	
	TArray<class UCItemData*> InventoryItems;
	TArray<class UCItemData*> InventoryEquipments;

protected:
	float MaxHP = 100.f;
	float HP = 100.f;
	float AttackPower = 0.f;
	float DefensePower = 0.f;
	float HealthRestorePower = 0.f;
	bool bHasWeapon = false;
public:
	TMap<int32, class UCItemData*> EquippedSlotStats;
	FINSERT_ITEM Delegate_INSERT_ITEM;
	FINSERT_ITEM* Delegate_UI_INSERT_ITEM;
	FINSERT_ITEM* Delegate_UI_INSERT_EQUIPMENT;
	FHP_CHANGED Delegate_HP_CHANGED;
	FOnStatusChanged Delegate_OnStatusChanged;
	FEQUIP_ITEM* Delegate_EquipItem;
	FUNEQUIP_ITEM* Delegate_UnEquipItem;

	virtual void BeginPlay() override;

	//virtual bool InsertItem(int32 ItemCategory, int32 ItemType, class UDataAsset* ItemData) override;
	virtual bool InsertItem(class UCItemData* ItemData, class UCItemData*& RtnItemData) override;
	virtual void RemoveItem(class UCItemData* ItemData) override;
	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) override;
	virtual void SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem) override;
	virtual void SetEquipDelegates(FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipDelegate) override;
	//virtual void SetUIEquipmentDelegate(FINSERT_ITEM* Delegate_InsertEquipment) override;
	virtual FHP_CHANGED* GetHPChangedDelegate() override { return &Delegate_HP_CHANGED; };
	virtual FOnStatusChanged* GetOnStatusChangedDelegate() override { return &Delegate_OnStatusChanged; };

	virtual void RecieveDamage(float DamageAmount) override;

	virtual float GetHP() { return HP; };
	virtual void SetHP(float e) {
		HP = e;
		Delegate_HP_CHANGED.ExecuteIfBound(MaxHP, HP);
	};
	virtual float GetMaxHP() { return MaxHP; };
	virtual void SetMaxHP(float e) { 
		MaxHP = e;
		Delegate_HP_CHANGED.ExecuteIfBound(MaxHP, HP);
	};
	virtual bool GetHasWeapon() override;
	virtual void SetHasWeapon(bool bHasWeapon) override;

	//UFUNCTION()
	//void InsertItem(class UCItemData* ItemData);
	bool InsertItemData(class UCItemData* ItemData, class UCItemData*& RtnItemData);

	virtual void RecalculateTotalStats() override;
	virtual class UCItemData* GetEquippedItemData(int32 ItemEquipSlot) override;

	UFUNCTION()
	void OnEquipItem(class UCItemData* ItemData);

	UFUNCTION()
	void OnUnEquipItem(class UCItemData* ItemData);
};
