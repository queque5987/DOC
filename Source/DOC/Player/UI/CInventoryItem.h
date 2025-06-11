#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Interfaces/IUIInventoryItem.h"
#include "CInventoryItem.generated.h"

UCLASS()
class DOC_API UCInventoryItem : public UUserWidget, public IUserObjectListEntry, public IIUIInventoryItem
{
	GENERATED_BODY()

	class UCItemData* ItemData;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCount;
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void RefreshUI() override;
};
