#include "CInventoryItem.h"
#include "Player/UI/CItemData.h"

void UCInventoryItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	ItemData = Cast<UCItemData>(ListItemObject);
	RefreshUI();
}

void UCInventoryItem::RefreshUI()
{
	if (ItemData == nullptr) return;
	if (ItemName != nullptr) ItemName->SetText(FText::FromName(ItemData->ItemName));
	if (ItemCount != nullptr) ItemCount->SetText(FText::FromString(FString::FromInt(ItemData->ItemCount)));
	if (ItemIcon != nullptr) ItemIcon->SetBrushFromTexture(ItemData->ItemIcon);
}
