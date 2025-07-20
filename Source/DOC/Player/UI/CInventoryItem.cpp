#include "CInventoryItem.h"
#include "Player/UI/CItemData.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UCInventoryItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	ItemData = Cast<UCItemData>(ListItemObject);
	RefreshUI();
}

void UCInventoryItem::RefreshUI()
{
	if (ItemData == nullptr) return;
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

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp, Log, TEXT("Inventory Item Clicked: %s"), *ItemData->ItemName.ToString());
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (OnEquipItemDelegatePtr != nullptr && OnEquipItemDelegatePtr->IsBound())
		{
			OnEquipItemDelegatePtr->Broadcast(ItemData);
		}
		else if (OnUnEquipItemDelegatePtr != nullptr && OnUnEquipItemDelegatePtr->IsBound())
		{
			OnUnEquipItemDelegatePtr->Broadcast(ItemData);
		}
	}

	return Reply;
}

void UCInventoryItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (OnItemHoveredDelegatePtr != nullptr)
	{
		OnItemHoveredDelegatePtr->ExecuteIfBound(ItemData);
	}

	UE_LOG(LogTemp, Log, TEXT("Mouse Entered: %s"), *ItemData->ItemName.ToString());
}

void UCInventoryItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (OnItemUnhoveredDelegatePtr != nullptr)
	{
		OnItemUnhoveredDelegatePtr->ExecuteIfBound();
	}

	UE_LOG(LogTemp, Log, TEXT("Mouse Left: %s"), *ItemData->ItemName.ToString());
}

void UCInventoryItem::SetDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate, FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipDelegate)
{
	OnItemHoveredDelegatePtr = HoveredDelegate;
	OnItemUnhoveredDelegatePtr = UnhoveredDelegate;
	OnEquipItemDelegatePtr = EquipDelegate;
	OnUnEquipItemDelegatePtr = UnEquipDelegate;
}
