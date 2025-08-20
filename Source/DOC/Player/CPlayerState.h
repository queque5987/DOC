#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IPlayerState.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Interfaces/CStageStructs.h"
#include "PCH.h"
#include "CPlayerState.generated.h"

UCLASS()
class DOC_API ACPlayerState : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()
	
	ACPlayerState();

	TArray<class UCItemData*> InventoryItems;

protected:
	FPlayerStat PlayerStat;
	FTimerHandle PlayerHealTimerHandle;
	float TickCounter = 0.f;
	float MPRegenTickCounter = 0.f;
	float HPRegenTickCounter = 0.f;
	const float MaxHealTickCounter = 1.f;
	const float MaxMPUseTickCounter = 0.5f;
	bool bHasWeapon = false;
	bool CounterReady = false;
public:

	UFUNCTION()
	void OnChangeCounterReady_Callback(bool bReady);
	TMap<int32, class UCItemData*> EquippedSlotStats;
	FINSERT_ITEM Delegate_INSERT_ITEM;
	FINSERT_ITEM* Delegate_UI_INSERT_ITEM;
	FINSERT_ITEM* Delegate_UI_INSERT_EQUIPMENT;
	//FHP_CHANGED Delegate_HP_CHANGED;
	FGETITEM Delegate_Get_Item;

	FOnStatusChanged Delegate_OnStatusChanged;
	FOnPlayerInventoryChanged Delegate_OnInventoryChanged;
	FEQUIP_ITEM* Delegate_EquipItem;
	FUNEQUIP_ITEM* Delegate_UnEquipItem;
	FOutOfMana* Delegate_OutOfMana;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//virtual bool InsertItem(int32 ItemCategory, int32 ItemType, class UDataAsset* ItemData) override;
	virtual void SetupDelegates(FOnChangeCounterReady* OnChangeCounterReady, FOutOfMana* OutOfMana) override;
	virtual bool InsertItem(class UCItemData* ItemData, class UCItemData*& RtnItemData) override;
	virtual void RemoveItem(class UCItemData* ItemData) override;
	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) override;
	virtual FOnPlayerInventoryChanged* GetOnInventoryChangedDelegate() override { return &Delegate_OnInventoryChanged; };
	virtual void SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem) override;
	virtual void SetEquipDelegates(FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipDelegate) override;
	virtual FOnStatusChanged* GetOnStatusChangedDelegate() override { return &Delegate_OnStatusChanged; };
	virtual FGETITEM* GetGetItemDelegate() override { return &Delegate_Get_Item; };
	virtual void RecieveDamage(float DamageAmount) override;

	virtual float GetHP() { return PlayerStat.CurrHP; };
	virtual void SetHP(float e) {
		PlayerStat.CurrHP = FMath::Min(e, PlayerStat.MaxHP);
		Delegate_OnStatusChanged.Broadcast(PlayerStat);
	};
	virtual float GetMaxHP() { return PlayerStat.MaxHP; };
	virtual void SetMaxHP(float e) { 
		PlayerStat.MaxHP = e;
		Delegate_OnStatusChanged.Broadcast(PlayerStat);
	};
	virtual float GetMP() { return PlayerStat.CurrMP; };
	virtual void SetMP(float e) {
		PlayerStat.CurrMP = FMath::Min(e, PlayerStat.MaxMP);
		MPRegenTickCounter = -1.f;
		if (PlayerStat.CurrMP < 0.f)
		{
			Delegate_OutOfMana->Broadcast();
			PlayerStat.CurrMP = 0.f;
		}
		Delegate_OnStatusChanged.Broadcast(PlayerStat);
	};
	virtual float GetMaxMP() { return PlayerStat.MaxMP; };
	virtual void SetMaxMP(float e) {
		PlayerStat.MaxMP = e;
		Delegate_OnStatusChanged.Broadcast(PlayerStat);
	};
	virtual bool GetHasWeapon() override;
	virtual void SetHasWeapon(bool bHasWeapon) override;
	virtual FPlayerStat GetPlayerStat() override;
	//UFUNCTION()
	//void InsertItem(class UCItemData* ItemData);
	bool InsertItemData(class UCItemData* ItemData, class UCItemData*& RtnItemData);

	virtual void RecalculateTotalStats() override;
	virtual class UCItemData* GetEquippedItemData(int32 ItemEquipSlot) override;

	UFUNCTION()
	void OnEquipItem(class UCItemData* ItemData);

	UFUNCTION()
	void OnUnEquipItem(class UCItemData* ItemData);
	virtual void SortInventoryItems() override;
	UFUNCTION()
	virtual void SimpleInsertItemData(class UCItemData* ItemData) override;
protected:
};
