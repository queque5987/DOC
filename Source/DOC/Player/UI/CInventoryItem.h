#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Interfaces/IUIInventoryItem.h"
#include "Interfaces/CStageDelegateTypes.h"
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

	FOnItemHovered* OnItemHoveredDelegatePtr;
	FOnItemUnhovered* OnItemUnhoveredDelegatePtr;

public:
	virtual void SetItemTooltipDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate) override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void RefreshUI() override;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
