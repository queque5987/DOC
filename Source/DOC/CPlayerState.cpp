#include "CPlayerState.h"
#include "Engine/DataAsset.h"
#include "CItemData.h"
#include "IUIInventory.h"

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
	Delegate_INSERT_ITEM.BindUFunction(this, TEXT("InsertItem"));
}

//void ACPlayerState::AddInventoryItem(UDataAsset* ItemData)
//{
//	UCItemData* temp = Cast<UCItemData>(ItemData);
//	InventoryItems.Add(temp);
//}

void ACPlayerState::GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem)
{
	Delegate_InsertItem = &Delegate_INSERT_ITEM;
}

void ACPlayerState::SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem)
{
	Delegate_UI_INSERT_ITEM = Delegate_InsertItem;
}


void ACPlayerState::InsertItem(UCItemData* ItemData)
{
	bool Flag = false;
	for (UCItemData* InventoryItem : InventoryItems)
	{
		if (InventoryItem->ItemCode == ItemData->ItemCode)
		{
			InventoryItem->ItemCount++;
			Delegate_UI_INSERT_ITEM->ExecuteIfBound(InventoryItem);
			Flag = true;
			break;
		}
	}
	if (Flag) return;
	else
	{
		FName ObjectName = FName(*(ItemData->ItemName.ToString() + FString::FromInt(ItemData->ItemCode)));
		UCItemData* tempData = DuplicateObject<UCItemData>(ItemData, this, ObjectName);
		if (tempData != nullptr)
		{
			InventoryItems.Add(tempData);
			Delegate_UI_INSERT_ITEM->ExecuteIfBound(tempData);
		}
	}
	int32 i = 0;
	for (UCItemData* InventoryItem : InventoryItems)
	{
		UE_LOG(LogTemp, Log, TEXT("ACPlayerState : InsertItem : %d\t%s"), i++, *InventoryItem->GetName());
	}
}
