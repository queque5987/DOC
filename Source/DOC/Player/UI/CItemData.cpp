#include "CItemData.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

UCItemData::UCItemData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> DragVisualBPClass(TEXT("/Game/UI/BP_InventoryItem_Drag")); // Replace with your actual blueprint path
	if (DragVisualBPClass.Class != nullptr)
	{
		DragVisualClass = DragVisualBPClass.Class;
	}
}

void UCItemData::Copy(UCItemData* ToCopyItemData)
{
    if (!ToCopyItemData)
    {
        return;
    }
    ItemName = ToCopyItemData->ItemName;
    ItemDescription = ToCopyItemData->ItemDescription;
    ItemCode = ToCopyItemData->ItemCode;
    ItemCategory = ToCopyItemData->ItemCategory;
    ItemRarity = ToCopyItemData->ItemRarity;
    ItemEquipSlot = ToCopyItemData->ItemEquipSlot;
    ItemCount = ToCopyItemData->ItemCount;
    bIsStackable = ToCopyItemData->bIsStackable;
    ItemUseSpawnParticle = ToCopyItemData->ItemUseSpawnParticle;
    ItemCoolDown = ToCopyItemData->ItemCoolDown;
    HealthToRestore = ToCopyItemData->HealthToRestore;
    Damage = ToCopyItemData->Damage;
    Defense = ToCopyItemData->Defense;
    CriticalRate = ToCopyItemData->CriticalRate;
    ItemActorClass = ToCopyItemData->ItemActorClass;
    Value = ToCopyItemData->Value;
    ItemIcon = ToCopyItemData->ItemIcon;
    DragVisualClass = ToCopyItemData->DragVisualClass;
}