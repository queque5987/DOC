#include "CPlayerState.h"
#include "Engine/DataAsset.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IUIInventory.h"

ACPlayerState::ACPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
    Delegate_Get_Item.AddUFunction(this, TEXT("SimpleInsertItemData"));
    Delegate_PlayerGroggyEnd.AddUFunction(this, TEXT("OnGroggyEnd"));
}

void ACPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    TickCounter += DeltaSeconds;
    HPRegenTickCounter += DeltaSeconds;
    MPRegenTickCounter += DeltaSeconds;
    if (CounterReady)
    {
        if (TickCounter >= MaxMPUseTickCounter)
        {
            SetMP(GetMP() - 6.f * TickCounter);
            TickCounter -= MaxMPUseTickCounter;
        }
    }
    else
    {
        if (TickCounter >= MaxMPUseTickCounter) TickCounter -= MaxMPUseTickCounter;
        bool StatusChangedFlag = false;
        if (HPRegenTickCounter >= MaxHealTickCounter)
        {
            StatusChangedFlag = true;
            PlayerStat.CurrHP += PlayerStat.HealthRegenPower;
            PlayerStat.CurrHP = FMath::Min(PlayerStat.MaxHP, PlayerStat.CurrHP);
            if (PlayerStat.Groggy > 1.f) PlayerStat.Groggy -= MaxHealTickCounter;
            else PlayerStat.Groggy = 0.f;
            HPRegenTickCounter -= MaxHealTickCounter;
        }
        if (MPRegenTickCounter >= MaxHealTickCounter)
        {
            StatusChangedFlag = true;

            PlayerStat.CurrMP += PlayerStat.MaxMP * 0.1f;
            PlayerStat.CurrMP = FMath::Min(PlayerStat.MaxMP, PlayerStat.CurrMP);

            MPRegenTickCounter -= MaxHealTickCounter;
        }
        if (StatusChangedFlag) Delegate_OnStatusChanged.Broadcast(PlayerStat);
    }
}

bool ACPlayerState::InsertItem(UCItemData* ItemData, UCItemData*& RtnItemData)
{
	return InsertItemData(ItemData, RtnItemData);
}

void ACPlayerState::GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem)
{
	//Delegate_InsertItem = &Delegate_INSERT_ITEM;
}

void ACPlayerState::SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem)
{
	//Delegate_UI_INSERT_ITEM = Delegate_InsertItem;
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
    PlayerStat.CurrHP -= FMath::Min(DamageAmount, PlayerStat.CurrHP);
    Delegate_OnStatusChanged.Broadcast(PlayerStat);
    HPRegenTickCounter = -4.f;
}

void ACPlayerState::RecieveDamage(FDamageConfig DamageConfig)
{
    PlayerStat.CurrHP -= FMath::Min(DamageConfig.Damage, PlayerStat.CurrHP);
    float tempGroggy = FMath::Clamp(PlayerStat.Groggy + DamageConfig.Groggy, 0.f, PlayerStat.MaxGroggy);
    PlayerStat.Groggy = tempGroggy;
    Delegate_OnStatusChanged.Broadcast(PlayerStat);
    if (PlayerStat.Groggy >= PlayerStat.MaxGroggy)
    {
        Delegate_PlayerGroggyOn.Broadcast(PlayerStat);
    }
    HPRegenTickCounter = -4.f;
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

FPlayerStat ACPlayerState::GetPlayerStat()
{
    return PlayerStat;
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
            float DamageRNG = FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);;
            float DefenseRNG = FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);;
            float HealthRestoreRNG = FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);;
            float CriticalRNG = FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);;
            float ValueRNG = 0.f;
            ValueRNG += tempData->Damage > 0.f ? DamageRNG : 1.f;
            ValueRNG += tempData->Defense > 0.f ? DefenseRNG : 1.f;
            ValueRNG += tempData->HealthToRestore > 0.f ? HealthRestoreRNG : 1.f;
            ValueRNG += tempData->CriticalRate > 0.f ? CriticalRNG : 1.f;
            tempData->Value *= ValueRNG / 4.f;
            tempData->Damage *= DamageRNG;
            tempData->Defense *= DefenseRNG;
            tempData->HealthToRestore *= HealthRestoreRNG;
            tempData->CriticalRate *= CriticalRNG;
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
    PlayerStat.AttackPower = 0.f;
    PlayerStat.DefencePower = 0.f;
    PlayerStat.HealthRegenPower = 0.f;
    PlayerStat.CriticalRate = 0.f;

	for (auto& Elem : EquippedSlotStats)
	{
        PlayerStat.AttackPower += Elem.Value->Damage;
        PlayerStat.DefencePower += Elem.Value->Defense;
        PlayerStat.HealthRegenPower += Elem.Value->HealthToRestore;
        PlayerStat.CriticalRate += Elem.Value->CriticalRate;
	}

    PlayerStat.CurrHP = FMath::Min(PlayerStat.CurrHP, PlayerStat.MaxHP);
    PlayerStat.CurrMP = FMath::Min(PlayerStat.CurrMP, PlayerStat.MaxMP);

    Delegate_OnStatusChanged.Broadcast(PlayerStat);
}

void ACPlayerState::OnEquipItem(UCItemData* ItemData)
{
	if (ItemData == nullptr) return;
    ItemData->Equipped = true;
	if (ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT) EquippedSlotStats.Add(ItemData->ItemEquipSlot, ItemData);

	RecalculateTotalStats();

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

void ACPlayerState::SimpleInsertItemData(UCItemData* ItemData)
{
    if (ItemData == nullptr) return;

    // Stackables
    if (ItemData->bIsStackable)
    {
        for (UCItemData* Data : InventoryItems)
        {
            if (Data->ItemCode == ItemData->ItemCode && Data->ItemCategory == ItemData->ItemCategory)
            {
                Data->AddItemCount();
                SortInventoryItems();
                return;
            }
        }
    }
    InventoryItems.Add(ItemData);
    SortInventoryItems();
}

void ACPlayerState::OnGroggyEnd()
{
    PlayerStat.Groggy = 0.f;
    Delegate_OnStatusChanged.Broadcast(PlayerStat);
}

void ACPlayerState::SetupDelegates(FOnChangeCounterReady* OnChangeCounterReady, FOutOfMana* OutOfMana)
{
	if (OnChangeCounterReady != nullptr)
	{
		OnChangeCounterReady->AddUFunction(this, FName("OnChangeCounterReady_Callback"));
	}
    Delegate_OutOfMana = OutOfMana;
}

void ACPlayerState::OnChangeCounterReady_Callback(bool bReady)
{
    if (CounterReady != bReady) MPRegenTickCounter = -2.f;
    CounterReady = bReady;
    TickCounter = 0.f;
}