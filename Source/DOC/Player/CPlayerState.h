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
	float MaxHP;
	float HP;
public:
	FINSERT_ITEM Delegate_INSERT_ITEM;
	FINSERT_ITEM* Delegate_UI_INSERT_ITEM;
	FINSERT_ITEM* Delegate_UI_INSERT_EQUIPMENT;
	FHP_CHANGED Delegate_HP_CHANGED;

	virtual void BeginPlay() override;

	//virtual bool InsertItem(int32 ItemCategory, int32 ItemType, class UDataAsset* ItemData) override;
	virtual bool InsertItem(class UCItemData* ItemData, class UCItemData*& RtnItemData) override;
	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) override;
	virtual void SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem) override;
	//virtual void SetUIEquipmentDelegate(FINSERT_ITEM* Delegate_InsertEquipment) override;
	virtual FHP_CHANGED* GetHPChangedDelegate() override { return &Delegate_HP_CHANGED; };

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

	//UFUNCTION()
	//void InsertItem(class UCItemData* ItemData);

	bool InsertItemData(class UCItemData* ItemData, class UCItemData*& RtnItemData);
};
