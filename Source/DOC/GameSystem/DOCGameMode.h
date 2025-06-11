#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/IGameModeDataManager.h"
#include "PCH.h"
#include "DOCGameMode.generated.h"

UCLASS(minimalapi)
class ADOCGameMode : public AGameModeBase, public IIGameModeDataManager
{
	GENERATED_BODY()

	TArray<class UCItemData*> ItemAssets;

public:
	ADOCGameMode();

	virtual class UCItemData* GetItemDataAsset(int32 ItemType) override;
};
