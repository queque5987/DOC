#include "CInventory.h"
#include "Components/Tileview.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IUIInventoryItem.h"
#include "Components/PanelWidget.h"

bool UCInventory::Initialize()
{
	bool rtn = Super::Initialize();
	Delegate_InsertItem.BindUFunction(this, TEXT("InsertItem"));

	ActiveButtonColorArr = {
		FLinearColor(0.91f, 0.41f, 0.55f, 1.f),
		FLinearColor(0.44f, 0.73f, 0.33f, 1.f),
		FLinearColor(0.52f, 0.91f, 0.74f, 1.f)
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

	// Etc Tab
	FInventoryTabInfo EtcTab;
	EtcTab.TabButton = Btn_Etc;
	EtcTab.TabText = Text_Btn_Etc;
	EtcTab.TabTileView = EtcTile;
	EtcTab.Category = ITEM_CATEGORY_ETC;
	Btn_Etc->OnClicked.AddDynamic(this, &UCInventory::OnEtcButtonClicked);
	TabInfos.Add(EtcTab);

	// Initial active tab setting
	SetActiveTab(ITEM_CATEGORY_EQUIPMENT);

	StatusTabInfos.Empty();

	// Equiped Tab
	FStatusTabInfo EquipedTab;
	EquipedTab.TabButton = Btn_Equiped;
	EquipedTab.TabText = Text_Btn_Equiped;
	EquipedTab.StatusPanel = EquippedPanel;
	EquipedTab.Category = 0;
	Btn_Equiped->OnClicked.AddDynamic(this, &UCInventory::OnEquipedButtonClicked);
	StatusTabInfos.Add(EquipedTab);

	// Stat Tab
	FStatusTabInfo StatTab;
	StatTab.TabButton = Btn_Stat;
	StatTab.TabText = Text_Btn_Stat;
	StatTab.StatusPanel = StatPanel;
	StatTab.Category = 1;
	Btn_Stat->OnClicked.AddDynamic(this, &UCInventory::OnStatButtonClicked);
	StatusTabInfos.Add(StatTab);

	// Quickslot Tab
	FStatusTabInfo QuickslotTab;
	QuickslotTab.TabButton = Btn_Quickslot;
	QuickslotTab.TabText = Text_Btn_Quickslot;
	QuickslotTab.StatusPanel = QuickslotPanel;
	QuickslotTab.Category = 2;
	Btn_Quickslot->OnClicked.AddDynamic(this, &UCInventory::OnQuickslotButtonClicked);
	StatusTabInfos.Add(QuickslotTab);

	SetActiveStatusTab(0);

	if (ItemTile != nullptr)
	{
		ItemTile->OnEntryWidgetGenerated().AddLambda([this](UUserWidget& Widget)
		{
			IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(&Widget);
			if (UIInventoryItem != nullptr)
			{
				UIInventoryItem->SetDelegates(OnItemHoveredDelegatePtr, OnItemUnhoveredDelegatePtr, OnEquipItemDelegatePtr, nullptr);
			}
		});
	}

	if (EtcTile != nullptr)
	{
		EtcTile->OnEntryWidgetGenerated().AddLambda([this](UUserWidget& Widget)
		{
			IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(&Widget);
			if (UIInventoryItem != nullptr)
			{
				UIInventoryItem->SetDelegates(OnItemHoveredDelegatePtr, OnItemUnhoveredDelegatePtr, nullptr, nullptr);
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
					UIInventoryItem->SetDelegates(OnItemHoveredDelegatePtr, OnItemUnhoveredDelegatePtr, OnEquipItemDelegatePtr, nullptr);
				}
			});
	}

	if (QuickslotTile != nullptr)
	{
		QuickslotTile->OnEntryWidgetGenerated().AddLambda([this](UUserWidget& Widget)
			{
				IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(&Widget);
				if (UIInventoryItem != nullptr)
				{
					UIInventoryItem->SetDelegates(OnItemHoveredDelegatePtr, OnItemUnhoveredDelegatePtr, OnEquipItemDelegatePtr, OnUnEquipItemDelegatePtr);
				}
			});
	}

	// Initialize EquipmentSlotTiles map and set up delegates
	if (WeaponTile != nullptr) { EquipmentSlotTiles.Add(EQUIPMENT_SWORD, WeaponTile); }
	if (HelmetTile != nullptr) { EquipmentSlotTiles.Add(EQUIPMENT_HELMET, HelmetTile); }
	if (GloveTile != nullptr) { EquipmentSlotTiles.Add(EQUIPMENT_GLOVE, GloveTile); }
	if (ShoseTile != nullptr) { EquipmentSlotTiles.Add(EQUIPMENT_SHOSE, ShoseTile); }
	if (TorsoTile != nullptr) { EquipmentSlotTiles.Add(EQUIPMENT_TORSO1, TorsoTile); }
	if (PantsTile != nullptr) { EquipmentSlotTiles.Add(EQUIPMENT_PANTS, PantsTile); }

	for (auto& Elem : EquipmentSlotTiles)
	{
		UTileView* CurrentTileView = Elem.Value;
		if (CurrentTileView != nullptr)
		{
			CurrentTileView->OnEntryWidgetGenerated().AddLambda([this](UUserWidget& Widget)
			{
				IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(&Widget);
				if (UIInventoryItem != nullptr)
				{
					UIInventoryItem->SetDelegates(OnItemHoveredDelegatePtr, OnItemUnhoveredDelegatePtr, nullptr, OnUnEquipItemDelegatePtr);
				}
			});
		}
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
	Refresh_ItemTile();
}

bool UCInventory::IsVisible()
{
	if (GetVisibility() != ESlateVisibility::Visible) return false;
	else return true;
}

void UCInventory::InsertItem(UCItemData* ItemData)
{
	if (ItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UCInventory :: InsertItem Iteam Data Nullptr"));
		return;
	}

	if (ItemData->ItemCategory == ITEM_CATEGORY_DISPOSABLE && ItemTile != nullptr)
	{
		ItemTile->AddItem(ItemData);
		UE_LOG(LogTemp, Log, TEXT("UCInventory :: Adding Item %s To Disposables"), ItemData->ItemName);
	}
	else if (ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT && EquipmentTile != nullptr)
	{
		EquipmentTile->AddItem(ItemData);
		UE_LOG(LogTemp, Log, TEXT("UCInventory :: Adding Item %s To Equipments"), ItemData->ItemName);
	}
	else UE_LOG(LogTemp, Error, TEXT("UCInventory :: Invalid Category %s"), ItemData->ItemName);
}

void UCInventory::RemoveItem(UCItemData* ItemData)
{
	if (ItemData == nullptr) return;

	UTileView* TargetTileView = nullptr;

	if (ItemData->ItemCategory == ITEM_CATEGORY_DISPOSABLE)
	{
		TargetTileView = ItemTile;
	}
	else if (ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT)
	{
		TargetTileView = EquipmentTile;
	}

	if (TargetTileView != nullptr)
	{
		TArray<UObject*> CurrentListItems = TargetTileView->GetListItems(); // Get a copy of the list

		// Find and remove the item
		int32 FoundIndex = INDEX_NONE;
		for (int32 i = 0; i < CurrentListItems.Num(); ++i)
		{
			UCItemData* CurrentItem = Cast<UCItemData>(CurrentListItems[i]);
			if (CurrentItem != nullptr && CurrentItem->ItemCode == ItemData->ItemCode)
			{
				FoundIndex = i;
				break;
			}
		}

		if (FoundIndex != INDEX_NONE)
		{
			// If stackable and count > 1, just decrease count, otherwise remove
			UCItemData* ItemToModify = Cast<UCItemData>(CurrentListItems[FoundIndex]);
			if (ItemToModify->bIsStackable && ItemToModify->ItemCount > 1)
			{
				ItemToModify->ItemCount--;
			}
			else
			{
				CurrentListItems.RemoveAt(FoundIndex);
			}
			TargetTileView->SetListItems(CurrentListItems); // Update the TileView's list with the modified copy
			TargetTileView->RequestRefresh(); // Request UI refresh
		}
	}
}

void UCInventory::EquipItem(UCItemData* ItemData)
{
	if (ItemData == nullptr) return;

	RemoveItem(ItemData);
	UTileView** TargetTileView = EquipmentSlotTiles.Find(ItemData->ItemEquipSlot);
	if (TargetTileView != nullptr && *TargetTileView != nullptr)
	{
		(*TargetTileView)->AddItem(ItemData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCInventory::EquipItem: No specific TileView found for ItemCode %d. Item not equipped to slot."), ItemData->ItemCode);
	}
	Refresh_ItemTile();
}

void UCInventory::UnEquipItem(UCItemData* ItemData)
{
	if (ItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UCInventory :: UnEquipItem : ItemData Nullptr"));
		return;
	}

	UTileView** TargetTileView = EquipmentSlotTiles.Find(ItemData->ItemEquipSlot);
	if (TargetTileView != nullptr && *TargetTileView != nullptr)
	{
		TArray<UObject*> CurrentListItems = (*TargetTileView)->GetListItems();
		int32 FoundIndex = INDEX_NONE;
		for (int32 i = 0; i < CurrentListItems.Num(); ++i)
		{
			UCItemData* CurrentItem = Cast<UCItemData>(CurrentListItems[i]);
			if (CurrentItem != nullptr && CurrentItem->ItemEquipSlot== ItemData->ItemEquipSlot)
			{
				FoundIndex = i;
				break;
			}
		}

		if (FoundIndex != INDEX_NONE)
		{
			CurrentListItems.RemoveAt(FoundIndex);
			(*TargetTileView)->SetListItems(CurrentListItems);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCInventory::UnEquipItem: No specific TileView found for ItemCode %d. Cannot unequip."), ItemData->ItemCode);
	}
	InsertItem(ItemData);
	Refresh_ItemTile();
}

void UCInventory::UnEquipItem(int32 ItemType)
{
	UTileView** TargetTileView = EquipmentSlotTiles.Find(ItemType);
	if (TargetTileView != nullptr && *TargetTileView != nullptr)
	{
		TArray<UObject*> CurrentListItems = (*TargetTileView)->GetListItems();
		if (CurrentListItems.Num() > 0)
		{
			UCItemData* ItemToUnEquip = Cast<UCItemData>(CurrentListItems[0]);
			if (ItemToUnEquip != nullptr)
			{
				CurrentListItems.RemoveAt(0);
				(*TargetTileView)->SetListItems(CurrentListItems);
				InsertItem(ItemToUnEquip);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCInventory::UnEquipItem(int32 ItemType): No specific TileView found for ItemType %d. Cannot unequip."), ItemType);
	}
	Refresh_ItemTile();
}

void UCInventory::Refresh_ItemTile()
{
	if (ItemTile != nullptr)
	{
		TArray<UUserWidget*> DisplayedWidgets = ItemTile->GetDisplayedEntryWidgets();
		for (UUserWidget* Widget : DisplayedWidgets)
		{
			IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(Widget);
			if (UIInventoryItem != nullptr)
			{
				UIInventoryItem->RefreshUI();
			}
		}
		ItemTile->RequestRefresh();
	}

	if (EquipmentTile != nullptr)
	{
		TArray<UUserWidget*> DisplayedWidgets = EquipmentTile->GetDisplayedEntryWidgets();
		for (UUserWidget* Widget : DisplayedWidgets)
		{
			IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(Widget);
			if (UIInventoryItem != nullptr)
			{
				UIInventoryItem->RefreshUI();
			}
		}
		EquipmentTile->RequestRefresh();
	}
}

int32 UCInventory::GetEmptyQuickSlotIndex() const
{
	if (QuickslotTile != nullptr)
	{
		TArray<UObject*> QuickslotItems = QuickslotTile->GetListItems();
		for (int32 i = 0; i < QuickslotItems.Num(); ++i)
		{
			UCItemData* ItemData = Cast<UCItemData>(QuickslotItems[i]);
			if (ItemData != nullptr && ItemData->ItemCode < 0)
			{
				return i;
			}
		}
	}
	return -1;
}

void UCInventory::ClearAllTileViews()
{
    if (ItemTile != nullptr)
    {
        ItemTile->ClearListItems();
    }
    if (EquipmentTile != nullptr)
    {
        EquipmentTile->ClearListItems();
    }
	if (QuickslotTile != nullptr)
	{
		QuickslotTile->ClearListItems();
	}
    for (auto& Elem : EquipmentSlotTiles)
    {
        if (Elem.Value != nullptr)
        {
            Elem.Value->ClearListItems();
        }
    }
}

void UCInventory::SetDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate, FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipItemDelegate, FOnStatusChanged* StatusChangedDelegate, FOnPlayerInventoryChanged* InventoryChangedDelegate, FOnQuickslotChanged* QuickslotChangedDelegate)
{
	OnItemHoveredDelegatePtr = HoveredDelegate;
	OnItemUnhoveredDelegatePtr = UnhoveredDelegate;
	OnEquipItemDelegatePtr = EquipDelegate;
	OnUnEquipItemDelegatePtr = UnEquipItemDelegate;
	OnStatusChangedDelegatePtr = StatusChangedDelegate;
	OnInventoryChangedDelegatePtr = InventoryChangedDelegate;
	OnQuickslotChangedDelegatePtr = QuickslotChangedDelegate;

	if (OnStatusChangedDelegatePtr != nullptr)
	{
		//OnStatusChangedDelegatePtr->BindUFunction(this, FName("OnStatusChanged"));
		OnStatusChangedDelegatePtr->AddUFunction(this, FName("OnStatusChanged"));
	}
	if (OnInventoryChangedDelegatePtr != nullptr)
	{
		OnInventoryChangedDelegatePtr->AddUFunction(this, FName("OnInventoryChanged"));
	}
}

void UCInventory::OnEquipmentButtonClicked() { SetActiveTab(ITEM_CATEGORY_EQUIPMENT); }
void UCInventory::OnDisposableButtonClicked() { SetActiveTab(ITEM_CATEGORY_DISPOSABLE); }

void UCInventory::OnEquipedButtonClicked()
{
	SetActiveStatusTab(0);
}

void UCInventory::OnStatButtonClicked()
{
	SetActiveStatusTab(1);
}

void UCInventory::OnEtcButtonClicked()
{
	SetActiveTab(ITEM_CATEGORY_ETC);
}

void UCInventory::OnQuickslotButtonClicked()
{
	SetActiveStatusTab(2);
}

void UCInventory::SetActiveStatusTab(int32 CategoryToActivate)
{
	for (FStatusTabInfo& TabInfo : StatusTabInfos)
	{
		bool bIsActive = (TabInfo.Category == CategoryToActivate);

		if (TabInfo.StatusPanel != nullptr)
		{
			TabInfo.StatusPanel->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
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
				UE_LOG(LogTemp, Warning, TEXT("UCInventory::SetActiveStatusTab: Invalid ActiveColor for category %d. Using fallback color."), TabInfo.Category);
			}

			ButtonStyle.Normal.TintColor = bIsActive ? CurrentActiveColor : InactiveButtonColor;
			//ButtonStyle.Hovered.TintColor = bIsActive ? CurrentActiveColor : InactiveButtonColor;
			//ButtonStyle.Pressed.TintColor = bIsActive ? CurrentActiveColor : InactiveButtonColor;
			TabInfo.TabButton->SetStyle(ButtonStyle);
		}

		if (TabInfo.TabText != nullptr)
		{
			TabInfo.TabText->SetColorAndOpacity(bIsActive ? ActiveTextColor : InactiveTextColor);
		}
	}
}

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

		if (TabInfo.TabText != nullptr)
		{
			TabInfo.TabText->SetColorAndOpacity(bIsActive ? ActiveTextColor : InactiveTextColor);
		}
	}
}

void UCInventory::OnStatusChanged(FPlayerStat PlayerStat)
{
	if (Equiped_Stat_Attack_Power != nullptr)
	{
		Equiped_Stat_Attack_Power->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), PlayerStat.AttackPower)));
	}
	if (Equiped_Stat_Defense_Power != nullptr)
	{
		Equiped_Stat_Defense_Power->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), PlayerStat.DefencePower)));
	}
	if (Equiped_Stat_Health_Regen_Power != nullptr)
	{
		Equiped_Stat_Health_Regen_Power->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), PlayerStat.HealthRegenPower)));
	}
	if (Equiped_Stat_Critical != nullptr)
	{
		Equiped_Stat_Critical->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), PlayerStat.CriticalRate)));
	}
}

void UCInventory::OnInventoryChanged(const TArray<class UCItemData*>& InventoryItemsArr)
{
	ClearAllTileViews();
	TArray<UCItemData*> QuickSlotTempArr;
	QuickSlotTempArr.SetNum(9); // Assuming 9 quickslots

	for (UCItemData* ItemData : InventoryItemsArr)
	{
		if (ItemData == nullptr) continue; // Skip null items from the source array

		if (ItemData->ItemCategory == ITEM_CATEGORY_DISPOSABLE)
		{
			if (ItemData->bIsStackable && ItemData->ItemCount < 1) continue;
			if (ItemData->Equipped)
			{
				if (ItemData->Quickslot >= 0 && ItemData->Quickslot < QuickSlotTempArr.Num())
				{
					QuickSlotTempArr[ItemData->Quickslot] = ItemData;
				}
			}
			ItemTile->AddItem(ItemData);
		}
		else if (ItemData->ItemCategory == ITEM_CATEGORY_ETC)
		{
			if (ItemData->bIsStackable && ItemData->ItemCount < 1) continue;
			EtcTile->AddItem(ItemData);
		}
		else if (ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT)
		{
			if (!ItemData->Equipped) EquipmentTile->AddItem(ItemData);
			else
			{
				if (EquipmentSlotTiles.Contains(ItemData->ItemEquipSlot))
				{
					EquipmentSlotTiles[ItemData->ItemEquipSlot]->AddItem(ItemData);
				}
			}
		}
	}

	// Fill empty quickslot entries with dummy data
	for (int32 i = 0; i < QuickSlotTempArr.Num(); ++i)
	{
		if (QuickSlotTempArr[i] == nullptr)
		{
			UCItemData* EmptyItemData = NewObject<UCItemData>(this); // Create a new empty item data object
			// Initialize empty item data properties
			EmptyItemData->ItemCode = -1;
			EmptyItemData->ItemName = FName("Empty Slot");
			EmptyItemData->ItemIcon = nullptr; // No icon for empty slot
			EmptyItemData->ItemCount = 0;
			EmptyItemData->bIsStackable = false;
			EmptyItemData->Equipped = false;
			EmptyItemData->ItemCategory = ITEM_CATEGORY_DISPOSABLE; // Or a specific category for empty slots
			EmptyItemData->ItemEquipSlot = -1;
			EmptyItemData->Quickslot = i;

			QuickSlotTempArr[i] = EmptyItemData;
		}
		QuickslotTile->AddItem(QuickSlotTempArr[i]);
	}

	if (OnQuickslotChangedDelegatePtr != nullptr)
	{
		OnQuickslotChangedDelegatePtr->Broadcast(QuickSlotTempArr);
	}
}
