#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameModeDataManager.generated.h"

UINTERFACE(MinimalAPI)
class UIGameModeDataManager : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIGameModeDataManager
{
	GENERATED_BODY()

public:
	virtual class UCItemData* GetItemDataAsset(int32 ItemType) { return nullptr; };
};
