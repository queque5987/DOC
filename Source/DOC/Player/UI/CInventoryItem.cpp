#include "CInventoryItem.h"
#include "Player/UI/CItemData.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Interfaces/CStageDelegateTypes.h" // 추가

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

FReply UCInventoryItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// 여기에 아이템 클릭 시 실행할 로직을 추가합니다.
		// 예: 아이템 사용, 정보 표시 등
		UE_LOG(LogTemp, Log, TEXT("Inventory Item Clicked: %s"), *ItemData->ItemName.ToString());
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

void UCInventoryItem::SetItemTooltipDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate)
{
	OnItemHoveredDelegatePtr = HoveredDelegate;
	OnItemUnhoveredDelegatePtr = UnhoveredDelegate;
}
