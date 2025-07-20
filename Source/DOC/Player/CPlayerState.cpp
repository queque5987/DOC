#include "CPlayerState.h"
#include "Engine/DataAsset.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IUIInventory.h"

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
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

void ACPlayerState::SetEquipDelegates(FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipDelegate)
{
	Delegate_EquipItem = EquipDelegate;
    Delegate_UnEquipItem = UnEquipDelegate;

	if (Delegate_EquipItem != nullptr)
    {
        Delegate_EquipItem->AddUFunction(this, FName("OnEquipItem"));
    }

	if (Delegate_UnEquipItem != nullptr)
	{
		Delegate_UnEquipItem->AddUFunction(this, FName("OnUnEquipItem"));
	}
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

void ACPlayerState::RemoveItem(UCItemData* ItemData)
{
    if (ItemData == nullptr)
    {
        return;
    }

    TArray<class UCItemData*>* TargetArray = nullptr;
    if (ItemData->ItemCategory == ITEM_CATEGORY_DISPOSABLE)
    {
        TargetArray = &InventoryItems;
    }
    else if (ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT)
    {
        TargetArray = &InventoryEquipments;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ACPlayerState::RemoveItem: Unknown ItemCategory: %d"), ItemData->ItemCategory);
        return;
    }

    if (TargetArray != nullptr)
    {
        for (int32 i = 0; i < TargetArray->Num(); ++i)
        {
            UCItemData* CurrentItem = (*TargetArray)[i];
            if (CurrentItem != nullptr && CurrentItem->ItemCode == ItemData->ItemCode)
            {
                if (CurrentItem->bIsStackable && CurrentItem->ItemCount > 1)
                {
                    CurrentItem->ItemCount--;
                }
                else
                {
                    TargetArray->RemoveAt(i);
                }
                return; // Item found and processed
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("ACPlayerState::RemoveItem: Item not found in inventory. ItemCode: %d"), ItemData->ItemCode);
}

bool ACPlayerState::GetHasWeapon()
{
	return bHasWeapon;
}

void ACPlayerState::SetHasWeapon(bool bHasWeaponIn)
{
	bHasWeapon = bHasWeaponIn;
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
            if (tempData->ItemCategory == ITEM_CATEGORY_EQUIPMENT)
            {
                int32 RarityRNG = ITEM_RARITY_NORMAL;
                if (FMath::FRand() > 0.5f)
                {
                    if (FMath::FRand() > 0.4f)
                    {
                        if (FMath::FRand() > 0.3f)
                        {
                            RarityRNG = ITEM_RARITY_LEGENDARY;
                        }
                        else
                        {
                            RarityRNG = ITEM_RARITY_EPIC;
                        }
                    }
                    else
                    {
                        RarityRNG = ITEM_RARITY_RARE;
                    }
                }
                tempData->ItemRarity = RarityRNG;

                tempData->Damage *= FMath::FRandRange(0.25f + RarityRNG * 0.75f, 1.f + RarityRNG * 0.75f);
                tempData->Defense *= FMath::FRandRange(0.25f + RarityRNG * 0.75f, 1.f + RarityRNG * 0.75f);
            }
            SearchArr->Add(tempData);
            RtnItemData = tempData;
            return true;
        }
    }

    return false;
}

void ACPlayerState::RecalculateTotalStats()
{
	AttackPower = 0.f;
	DefensePower = 0.f;
    HealthRestorePower = 0.f;

	for (auto& Elem : EquippedSlotStats)
	{
		AttackPower += Elem.Value->Damage;
		DefensePower += Elem.Value->Defense;
        HealthRestorePower += Elem.Value->HealthToRestore;
	}
    Delegate_OnStatusChanged.ExecuteIfBound(AttackPower, DefensePower, HealthRestorePower);
}

void ACPlayerState::OnEquipItem(UCItemData* ItemData)
{
	if (ItemData == nullptr) return;

	EquippedSlotStats.Add(ItemData->ItemEquipSlot, ItemData);

	RecalculateTotalStats();

	UE_LOG(LogTemp, Log, TEXT("Equipped Item: %s, Attack: %f, Defense: %f"), *ItemData->ItemName.ToString(), AttackPower, DefensePower);
}

void ACPlayerState::OnUnEquipItem(UCItemData* ItemData)
{
	if (ItemData == nullptr) return;

	EquippedSlotStats.Remove(ItemData->ItemEquipSlot);

	RecalculateTotalStats();

	UE_LOG(LogTemp, Log, TEXT("Unequipped Item: %s"), *ItemData->ItemName.ToString());
}

UCItemData* ACPlayerState::GetEquippedItemData(int32 ItemEquipSlot)
{
	if (EquippedSlotStats.Contains(ItemEquipSlot))
	{
		return EquippedSlotStats[ItemEquipSlot];
	}
	return nullptr;
}
