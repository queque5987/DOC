#include "CInventory.h"
#include "Components/Tileview.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IUIInventoryItem.h"

bool UCInventory::Initialize()
{
	bool rtn = Super::Initialize();
	Delegate_InsertItem.BindUFunction(this, TEXT("InsertItem"));

	ActiveButtonColorArr = {
		FLinearColor(0.91f, 0.41f, 0.55f, 1.f),
		FLinearColor(0.44f, 0.73f, 0.33f, 1.f)
	};

	TabInfos.Empty();

	// Equipment Tab
	FInventoryTabInfo EquipmentTab;
	EquipmentTab.TabButton = Btn_Equipment;
	EquipmentTab.TabText = Text_Btn_Equipment;
	EquipmentTab.TabTileView = EquipmentTile;
	EquipmentTab.Category = ITEM_CATEGORY_EQUIPMENT;
	Btn_Equipment->OnClicked.AddDynamic(this, &UCInventory::OnEquipmentButtonClicked);
	TabInfos.Add(EquipmentTab);

	// Disposable Tab
	FInventoryTabInfo DisposableTab;
	DisposableTab.TabButton = Btn_Desposable;
	DisposableTab.TabText = Text_Btn_Desposable;
	DisposableTab.TabTileView = ItemTile;
	DisposableTab.Category = ITEM_CATEGORY_DISPOSABLE;
	Btn_Desposable->OnClicked.AddDynamic(this, &UCInventory::OnDisposableButtonClicked);
	TabInfos.Add(DisposableTab);

	// Initial active tab setting
	SetActiveTab(ITEM_CATEGORY_EQUIPMENT);

	if (ItemTile != nullptr)
	{
		ItemTile->OnEntryWidgetGenerated().AddLambda([this](UUserWidget& Widget)
		{
			IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(&Widget);
			if (UIInventoryItem != nullptr)
			{
				UIInventoryItem->SetItemTooltipDelegates(OnItemHoveredDelegatePtr, OnItemUnhoveredDelegatePtr);
			}
		});
	}

	if (EquipmentTile != nullptr)
	{
		EquipmentTile->OnEntryWidgetGenerated().AddLambda([this](UUserWidget& Widget)
			{
				IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(&Widget);
				if (UIInventoryItem != nullptr)
				{
					UIInventoryItem->SetItemTooltipDelegates(OnItemHoveredDelegatePtr, OnItemUnhoveredDelegatePtr);
				}
			});
	}
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
	if (ItemData == nullptr) return;

	if (ItemData->ItemCategory == ITEM_CATEGORY_DISPOSABLE && ItemTile != nullptr)
	{
		ItemTile->AddItem(ItemData);
	}
	else if (ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT && EquipmentTile != nullptr)
	{
		EquipmentTile->AddItem(ItemData);
	}
	 Refresh_ItemTile();
}

void UCInventory::Refresh_ItemTile()
{
	ItemTile->RequestRefresh();
	EquipmentTile->RequestRefresh();
}

void UCInventory::SetItemTooltipDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate)
{
	OnItemHoveredDelegatePtr = HoveredDelegate;
	OnItemUnhoveredDelegatePtr = UnhoveredDelegate;
}

 void UCInventory::OnEquipmentButtonClicked() { SetActiveTab(ITEM_CATEGORY_EQUIPMENT); }
 void UCInventory::OnDisposableButtonClicked() { SetActiveTab(ITEM_CATEGORY_DISPOSABLE); }

void UCInventory::SetActiveTab(int32 CategoryToActivate)
{
	for (FInventoryTabInfo& TabInfo : TabInfos)
	{
		bool bIsActive = (TabInfo.Category == CategoryToActivate);

		if (TabInfo.TabTileView != nullptr)
		{
			TabInfo.TabTileView->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}

		if (TabInfo.TabButton != nullptr)
		{
			FButtonStyle ButtonStyle = TabInfo.TabButton->WidgetStyle;
			FLinearColor CurrentActiveColor = FLinearColor::Blue; // Fallback color

			if (ActiveButtonColorArr.IsValidIndex(TabInfo.Category))
			{
				CurrentActiveColor = ActiveButtonColorArr[TabInfo.Category];
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UCInventory::SetActiveTab: Invalid ActiveColor for category %d. Using fallback color."), TabInfo.Category);
			}

			ButtonStyle.Normal.TintColor = bIsActive ? CurrentActiveColor : InactiveButtonColor;
			//ButtonStyle.Hovered.TintColor = bIsActive ? CurrentActiveColor : InactiveButtonColor;
			//ButtonStyle.Pressed.TintColor = bIsActive ? CurrentActiveColor : InactiveButtonColor;
			TabInfo.TabButton->SetStyle(ButtonStyle);
		}

		// 텍스트 색상 변경
		if (TabInfo.TabText != nullptr)
		{
			TabInfo.TabText->SetColorAndOpacity(bIsActive ? ActiveTextColor : InactiveTextColor);
		}
	}
}
