#include "CInventory.h"
#include "Components/Tileview.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IUIInventoryItem.h"

bool UCInventory::Initialize()
{
	bool rtn = Super::Initialize();
	Delegate_InsertItem.BindUFunction(this, TEXT("InsertItem"));
	return rtn;
}

void UCInventory::SetVisibility(ESlateVisibility InVisibility)
{
	if (Disabled) return;

	Super::SetVisibility(InVisibility);
	Disabled = true;
	GetWorld()->GetTimerManager().SetTimer(DisableTimer, FTimerDelegate::CreateLambda(
		[&] {
			Disabled = false;
		}
	), DisableSec, false);
}

bool UCInventory::IsVisible()
{
	if (GetVisibility() != ESlateVisibility::Visible) return false;
	else return true;
}

void UCInventory::InsertItem(UCItemData* ItemData)
{
	if (ItemTile->GetIndexForItem(ItemData) < 0) ItemTile->AddItem(ItemData);
	else
	{
		UUserWidget* tempItemUI = ItemTile->GetEntryWidgetFromItem(ItemData);
		if (tempItemUI != nullptr)
		{
			IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(tempItemUI);
			if (UIInventoryItem != nullptr) UIInventoryItem->RefreshUI();
		}
	}
}

void UCInventory::Refresh_ItemTile()
{
	ItemTile->RequestRefresh();
}
