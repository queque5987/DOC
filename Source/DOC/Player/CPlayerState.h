#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IPlayerState.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CPlayerState.generated.h"

UCLASS()
class DOC_API ACPlayerState : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()
	
	TArray<class UCItemData*> InventoryItems;

protected:
	float MaxHP;
	float HP;
public:
	FINSERT_ITEM Delegate_INSERT_ITEM;
	FINSERT_ITEM* Delegate_UI_INSERT_ITEM;
	FHP_CHANGED Delegate_HP_CHANGED;

	virtual void BeginPlay() override;

	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) override;
	virtual void SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem) override;
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

	UFUNCTION()
	void InsertItem(class UCItemData* ItemData);

};
