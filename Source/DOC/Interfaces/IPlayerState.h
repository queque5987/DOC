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
	//virtual bool InsertItem(int32 ItemCategory, int32 ItemType, class UDataAsset* ItemData) { return bool(); };
	virtual bool InsertItem(class UCItemData* ItemData, class UCItemData*& RtnItemData) { return bool(); };
	virtual void SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem) {};
	virtual FHP_CHANGED* GetHPChangedDelegate() { return nullptr; };

	virtual void RecieveDamage(float DamageAmount) {};
	virtual float GetHP() { return 0.f; };
	virtual float GetMaxHP() { return 0.f; };
	virtual void SetHP(float e) {};
	virtual void SetMaxHP(float e) {};
};
