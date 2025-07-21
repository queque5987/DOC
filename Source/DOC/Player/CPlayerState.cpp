#include "CPlayerState.h"
#include "Engine/DataAsset.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IUIInventory.h"

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

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
    if (ItemData == nullptr) return;

    for (int32 i = 0; i < InventoryItems.Num(); ++i)
    {
        UCItemData* CurrentItem = InventoryItems[i];
        if (CurrentItem != nullptr && CurrentItem->ItemCode == ItemData->ItemCode)
        {
            if (CurrentItem->bIsStackable && CurrentItem->ItemCount > 1)
            {
                CurrentItem->ItemCount--;
            }
            else
            {
                InventoryItems.RemoveAt(i);
            }
            SortInventoryItems();
            return;
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

bool ACPlayerState::InsertItemData(UCItemData* ItemData, UCItemData*& RtnItemData)
{
    RtnItemData = nullptr;
    if (ItemData == nullptr)
    {
        return false;
    }
    int32 ItemCategory = ItemData->ItemCategory;


    // Stackables
    if (ItemData->bIsStackable)
    {
        for (UCItemData* Data : InventoryItems)
        {
            if (Data->ItemCode == ItemData->ItemCode)
            {
                Data->AddItemCount();
                SortInventoryItems();
                return true;
            }
        }
    }
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

            tempData->Damage *= FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);
            tempData->Defense *= FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);
            tempData->HealthToRestore *= FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);
        }
        tempData->Equipped = false;
        InventoryItems.Add(tempData);
        RtnItemData = tempData;
        SortInventoryItems();
        return true;
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
    ItemData->Equipped = true;
	EquippedSlotStats.Add(ItemData->ItemEquipSlot, ItemData);

	RecalculateTotalStats();

	UE_LOG(LogTemp, Log, TEXT("Equipped Item: %s, Attack: %f, Defense: %f"), *ItemData->ItemName.ToString(), AttackPower, DefensePower);
    SortInventoryItems();
}

void ACPlayerState::OnUnEquipItem(UCItemData* ItemData)
{
	if (ItemData == nullptr) return;
    ItemData->Equipped = false;
	EquippedSlotStats.Remove(ItemData->ItemEquipSlot);

	RecalculateTotalStats();

	UE_LOG(LogTemp, Log, TEXT("Unequipped Item: %s"), *ItemData->ItemName.ToString());
    SortInventoryItems();
}

UCItemData* ACPlayerState::GetEquippedItemData(int32 ItemEquipSlot)
{
	if (EquippedSlotStats.Contains(ItemEquipSlot))
	{
		return EquippedSlotStats[ItemEquipSlot];
	}
	return nullptr;
}

void ACPlayerState::SortInventoryItems()
{
    InventoryItems.Sort([](const UCItemData& A, const UCItemData& B)
    {
        if (A.ItemCode != B.ItemCode)
        {
            return A.ItemCode < B.ItemCode;
        }

        if (A.ItemRarity != B.ItemRarity)
        {
            return A.ItemRarity > B.ItemRarity;
        }

        float StatA = A.Damage + A.Defense + A.HealthToRestore;
        float StatB = B.Damage + B.Defense + B.HealthToRestore;
        return StatA > StatB;
    });

    Delegate_OnInventoryChanged.Broadcast(InventoryItems);
}
