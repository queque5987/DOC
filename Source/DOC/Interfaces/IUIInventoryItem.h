#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "IUIInventoryItem.generated.h"


UINTERFACE(MinimalAPI)
class UIUIInventoryItem : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIUIInventoryItem
{
	GENERATED_BODY()

public:
	virtual void RefreshUI() {};
	virtual void SetItemTooltipDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate) {};
};
