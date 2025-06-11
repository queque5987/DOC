#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IUIInventory.generated.h"

UINTERFACE(MinimalAPI)
class UIUIInventory : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIUIInventory
{
	GENERATED_BODY()

public:
	virtual void InsertItem() {};
};
