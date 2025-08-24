#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "PCH.h"
#include "DOCGameMode.generated.h"

UCLASS(minimalapi)
class ADOCGameMode : public AGameModeBase
{
	GENERATED_BODY()

	TArray<class UCItemData*> ItemAssets;
	TArray<class UCItemData*> EquipmentAssets;
public:
	ADOCGameMode();
	//virtual class UCItemData* GetItemDataAsset(int32 ItemType) override;
	//virtual class UCItemData* GetEquipmentDataAsset(int32 EquipmentType) override;
};
