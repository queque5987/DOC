#pragma once

#include "CoreMinimal.h"
#include "Player/UI/CInventoryItem.h"
#include "CInventoryItem_E.generated.h"

UCLASS()
class DOC_API UCInventoryItem_E : public UCInventoryItem
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* GetEffectBG;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

};
