#include "CPlayerState.h"
#include "Engine/DataAsset.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IUIInventory.h"

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
	//Delegate_INSERT_ITEM.BindUFunction(this, TEXT("InsertItem"));
}

//void ACPlayerState::AddInventoryItem(UDataAsset* ItemData)
//{
//	UCItemData* temp = Cast<UCItemData>(ItemData);
//	InventoryItems.Add(temp);
//}

bool ACPlayerState::InsertItem(UCItemData* ItemData, UCItemData*& RtnItemData)
{
	return InsertItemData(ItemData, RtnItemData);
}

void ACPlayerState::GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem)
{
	Delegate_InsertItem = &Delegate_INSERT_ITEM;
}

void ACPlayerState::SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem)
{
	Delegate_UI_INSERT_ITEM = Delegate_InsertItem;
}

//void ACPlayerState::SetUIEquipmentDelegate(FINSERT_ITEM* Delegate_InsertEquipment)
//{
//	Delegate_UI_INSERT_EQUIPMENT = Delegate_InsertEquipment;
//}

void ACPlayerState::RecieveDamage(float DamageAmount)
{
	HP -= DamageAmount;
	Delegate_HP_CHANGED.ExecuteIfBound(MaxHP, HP);
}


//void ACPlayerState::InsertItem(UCItemData* ItemData)
//{
//	bool Flag = false;
//	for (UCItemData* InventoryItem : InventoryItems)
//	{
//		if (InventoryItem->ItemCode == ItemData->ItemCode)
//		{
//			InventoryItem->ItemCount++;
//			Delegate_UI_INSERT_ITEM->ExecuteIfBound(InventoryItem);
//			Flag = true;
//			break;
//		}
//	}
//	if (Flag) return;
//	else
//	{
//		FName ObjectName = FName(*(ItemData->ItemName.ToString() + FString::FromInt(ItemData->ItemCode)));
//		UCItemData* tempData = DuplicateObject<UCItemData>(ItemData, this, ObjectName);
//		if (tempData != nullptr)
//		{
//			InventoryItems.Add(tempData);
//			Delegate_UI_INSERT_ITEM->ExecuteIfBound(tempData);
//		}
//	}
//	int32 i = 0;
//	for (UCItemData* InventoryItem : InventoryItems)
//	{
//		UE_LOG(LogTemp, Log, TEXT("ACPlayerState : InsertItem : %d\t%s"), i++, *InventoryItem->GetName());
//	}
//}

bool ACPlayerState::InsertItemData(UCItemData* ItemData, UCItemData*& RtnItemData)
{
    RtnItemData = nullptr;
    if (ItemData == nullptr)
    {
        return false;
    }
    int32 ItemCategory = ItemData->ItemCategory;
    TArray<class UCItemData*>* SearchArr = nullptr;

    // To Add TileView
    switch (ItemCategory)
    {
    case(ITEM_CATEGORY_DISPOSABLE):
        SearchArr = &InventoryItems;
        break;
    case(ITEM_CATEGORY_EQUIPMENT):
        SearchArr = &InventoryEquipments;
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("ACPlayerState::InsertItemData: Unknown ItemCategory: %d"), ItemCategory);
        return false;
    }

    // Stackables
    if (ItemData->bIsStackable)
    {
        if (SearchArr != nullptr)
        {
            for (UCItemData* iterItem : *SearchArr)
            {
                if (iterItem && iterItem->ItemCode == ItemData->ItemCode)
                {
                    iterItem->AddItemCount();
                    return true;
                }
            }
        }
    }

    // Unstackable or Firstly Added
    if (SearchArr != nullptr)
    {
        FName ObjectName = FName(FString::Printf(TEXT("%s_%d_%f"), *(ItemData->ItemName.ToString()), ItemData->ItemCode, GetWorld()->GetTimeSeconds()));
        UCItemData* tempData = DuplicateObject<UCItemData>(ItemData, this, ObjectName);
        
        if (tempData != nullptr)
        {
            SearchArr->Add(tempData);
            RtnItemData = tempData;
            return true;
        }
    }

    return false;
}
