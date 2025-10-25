#include "CInventoryItem.h"
#include "Player/UI/CItemData.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "UObject/ConstructorHelpers.h"

UCInventoryItem::UCInventoryItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UCInventoryItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	ItemData = Cast<UCItemData>(ListItemObject);
	if (ItemData != nullptr) UE_LOG(LogTemp, Log, TEXT("UCInventoryItem : NativeOnListItemObjectSet : %s"), *ItemData->ItemName.ToString())
	RefreshUI();
}

void UCInventoryItem::NativeOnEntryReleased()
{
	if (ItemData != nullptr) UE_LOG(LogTemp, Log, TEXT("UCInventoryItem : NativeOnEntryReleased : %s"), *ItemData->ItemName.ToString())
}

void UCInventoryItem::RefreshUI()
{
	if (ItemData == nullptr) return;

	if (SizePanel != nullptr)
	{
		if (ItemData->ItemCode < 0)
		{
			SizePanel->SetRenderOpacity(0.2f);
			CountPanel->SetVisibility(ESlateVisibility::Collapsed);
			ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}
		else
		{
			SizePanel->SetRenderOpacity(1.f);
			CountPanel->SetVisibility(ESlateVisibility::Visible);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (ItemCount != nullptr) ItemCount->SetText(FText::FromString(FString::FromInt(ItemData->ItemCount)));
	if (ItemIcon != nullptr) ItemIcon->SetBrushFromTexture(ItemData->ItemIcon);

	if (CountPanel != nullptr)
	{
		CountPanel->SetVisibility(ItemData->bIsStackable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (ImageItemRarity != nullptr)
	{
		FLinearColor RarityColor;
		switch (ItemData->ItemRarity)
		{
		case ITEM_RARITY_NORMAL:
			RarityColor = ITEM_COLOR_NORMAL;
			break;
		case ITEM_RARITY_RARE:
			RarityColor = ITEM_COLOR_RARE;
			break;
		case ITEM_RARITY_EPIC:
			RarityColor = ITEM_COLOR_EPIC;
			break;
		case ITEM_RARITY_LEGENDARY:
			RarityColor = ITEM_COLOR_LEGENDARY;
			break;
		default:
			RarityColor = FLinearColor::White;
			break;
		}
		ImageItemRarity->SetColorAndOpacity(RarityColor);
	}
}

FReply UCInventoryItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (ItemData->ItemCode < 0) return Reply;

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp, Log, TEXT("Inventory Item Clicked: %s"), *ItemData->ItemName.ToString());
		Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if ((OnEquipItemDelegatePtr != nullptr && OnEquipItemDelegatePtr->IsBound()) && (ItemData != nullptr && !ItemData->Equipped))
		{
			OnEquipItemDelegatePtr->Broadcast(ItemData);
		}
		else if (OnUnEquipItemDelegatePtr != nullptr && OnUnEquipItemDelegatePtr->IsBound())
		{
			OnUnEquipItemDelegatePtr->Broadcast(ItemData);
		}
		else if (OnGetItemDelegatePtr != nullptr && OnGetItemDelegatePtr->IsBound())
		{
			OnGetItemDelegatePtr->Broadcast(ItemData);
		}
	}

	return Reply;
}

void UCInventoryItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (ItemData == nullptr || ItemData->ItemCode < 0) return;

	if (OnItemHoveredDelegatePtr != nullptr)
	{
		OnItemHoveredDelegatePtr->ExecuteIfBound(ItemData);
	}

	UE_LOG(LogTemp, Log, TEXT("Mouse Entered: %s"), *ItemData->ItemName.ToString());
}

void UCInventoryItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (ItemData->ItemCode < 0) return;

	if (OnItemUnhoveredDelegatePtr != nullptr)
	{
		OnItemUnhoveredDelegatePtr->ExecuteIfBound();
	}

	UE_LOG(LogTemp, Log, TEXT("Mouse Left: %s"), *ItemData->ItemName.ToString());
}

void UCInventoryItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (ItemData->ItemCode < 0) return;

	if (ItemData != nullptr)
	{
		UDragDropOperation* DragDropOp = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
		if (DragDropOp != nullptr)
		{
			UCInventoryItem* DragVisual = CreateWidget<UCInventoryItem>(this, ItemData->DragVisualClass);
			if (DragVisual != nullptr)
			{
				DragVisual->ItemData = ItemData; // Pass the item data to the visual
				DragVisual->RefreshUI(); // Refresh its UI to show the item
			//	DragVisual->SetRenderOpacity(0.7f); // Make it translucent
			//	DragVisual->SetDesiredSizeInViewport(InGeometry.GetLocalSize()); // Set the size
			//	
				DragDropOp->DefaultDragVisual = DragVisual;
			}
			//DragDropOp->DefaultDragVisual = this;
			
			DragDropOp->Payload = ItemData; // Set the actual item data as payload
			DragDropOp->Pivot = EDragPivot::CenterCenter; // Center the visual on the cursor

			OutOperation = DragDropOp; // Assign the created operation to OutOperation
		}
	}
}

void UCInventoryItem::SetDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate, FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipDelegate)
{
	OnItemHoveredDelegatePtr = HoveredDelegate;
	OnItemUnhoveredDelegatePtr = UnhoveredDelegate;
	OnEquipItemDelegatePtr = EquipDelegate;
	OnUnEquipItemDelegatePtr = UnEquipDelegate;
	OnGetItemDelegatePtr = nullptr;
}

void UCInventoryItem::SetDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate, FGETITEM* GetItemDelegate)
{
	OnItemHoveredDelegatePtr = HoveredDelegate;
	OnItemUnhoveredDelegatePtr = UnhoveredDelegate;
	OnEquipItemDelegatePtr = nullptr;
	OnUnEquipItemDelegatePtr = nullptr;
	OnGetItemDelegatePtr = GetItemDelegate;
}

bool UCInventoryItem::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (InOperation && InOperation->Payload)
	{
		UCItemData* DroppedItemData = Cast<UCItemData>(InOperation->Payload);
		if (DroppedItemData && ItemData)
		{
			UE_LOG(LogTemp, Log, TEXT("Item Dropped: %s"), *DroppedItemData->ItemName.ToString());
			DroppedItemData->Quickslot = ItemData->Quickslot;
			DroppedItemData->Equipped = true;
			if (OnEquipItemDelegatePtr != nullptr) OnEquipItemDelegatePtr->Broadcast(DroppedItemData);
			bHandled = true;
		}
	}

	return bHandled;
}
