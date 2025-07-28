#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Interfaces/IUIInventoryItem.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "PCH.h"
#include "CInventoryItem.generated.h"

UCLASS()
class DOC_API UCInventoryItem : public UUserWidget, public IUserObjectListEntry, public IIUIInventoryItem
{
	GENERATED_BODY()

	class UCItemData* ItemData;
	UPROPERTY(meta = (BindWidget))
	class UImage* ImageItemRarity;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCount;
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* CountPanel;
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* SizePanel;

	FOnItemHovered* OnItemHoveredDelegatePtr;
	FOnItemUnhovered* OnItemUnhoveredDelegatePtr;
	FEQUIP_ITEM* OnEquipItemDelegatePtr;
	FUNEQUIP_ITEM* OnUnEquipItemDelegatePtr;

public:
	UCInventoryItem(const FObjectInitializer& ObjectInitializer);
	virtual void SetDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate, FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipDelegate) override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void RefreshUI() override;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
