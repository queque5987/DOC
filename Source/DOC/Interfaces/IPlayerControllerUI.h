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
	virtual bool InsertItem(FINSERT_ITEM*& Delegate_InsertItem, class AActor* Item, int32 ItemType) { return bool(); };
	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) {};
	virtual void ToggleWidemap(bool e) {};
	virtual void ToggleMinimap(bool e) {};
};
