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
	virtual void SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem) {};
};
