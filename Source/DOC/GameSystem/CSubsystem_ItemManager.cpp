#include "GameSystem/CSubsystem_ItemManager.h"
#include "Player/UI/CItemData.h"
#include "UObject/ConstructorHelpers.h"

UCSubsystem_ItemManager::UCSubsystem_ItemManager()
{
	TArray<TObjectPtr<UCItemData>> FoundAssets;

	auto FindAndAddAsset = [&FoundAssets](const TCHAR* Path) {
		ConstructorHelpers::FObjectFinder<UCItemData> Finder(Path);
		if (Finder.Succeeded())
		{
			FoundAssets.Add(Finder.Object);
		}
	};
	
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Disposable/DA_Potion1.DA_Potion1"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Disposable/DA_Potion2.DA_Potion2"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Guitar/DA_Gemstone1.DA_Gemstone1"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Guitar/DA_Gemstone2.DA_Gemstone2"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Guitar/DA_Gemstone3.DA_Gemstone3"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Guitar/DA_Gemstone4.DA_Gemstone4"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Guitar/DA_Gemstone5.DA_Gemstone5"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Guitar/DA_Gemstone6.DA_Gemstone6"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Sword.DA_Sword"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Glove.DA_Glove"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Mask.DA_Mask"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Pants.DA_Pants"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Shose.DA_Shose"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Torso1.DA_Torso1"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Torso2.DA_Torso2"));
	FindAndAddAsset(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Torso3.DA_Torso3"));

	CategorizedItemData.Empty();
	for (const auto& DataAsset : FoundAssets)
	{
		if (DataAsset && DataAsset->ItemCategory >= 0 && DataAsset->ItemCode >= 0)
		{
			CategorizedItemData.FindOrAdd(DataAsset->ItemCategory).Add(DataAsset->ItemCode, DataAsset);
		}
	}
}

void UCSubsystem_ItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

UCItemData* UCSubsystem_ItemManager::GetOrCreateItemInstance(int32 ItemCategory, int32 ItemCode)
{
	const FIntPoint ItemKey(ItemCategory, ItemCode);
	bool IsEquipment = ItemCategory == ITEM_CATEGORY_EQUIPMENT;

	UCItemData* BaseDataAsset = FindDataAsset(ItemCategory, ItemCode);
	if (BaseDataAsset != nullptr)
	{
		UCItemData* NewInstance = NewObject<UCItemData>(this);
		if(NewInstance)
		{
			NewInstance->Copy(BaseDataAsset);
			if (IsEquipment) RNGEquipmentStats(NewInstance);
			InstancedItemCache.FindOrAdd(ItemKey).Add(NewInstance);
			InstancedItemArr.Add(NewInstance);
			return NewInstance;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Could not create instance for item (%d, %d)"), ItemCategory, ItemCode);
	return nullptr;
}

UCItemData* UCSubsystem_ItemManager::FindDataAsset(int32 ItemCategory, int32 ItemCode) const
{
	const TMap<int32, TObjectPtr<UCItemData>>* CategoryMap = CategorizedItemData.Find(ItemCategory);
	if (CategoryMap)
	{
		TObjectPtr<UCItemData> const* FoundItemPtr = CategoryMap->Find(ItemCode);
		if (FoundItemPtr)
		{
			return *FoundItemPtr;
		}
	}
	return nullptr;
}

void UCSubsystem_ItemManager::RNGEquipmentStats(UCItemData*& RNGItemData)
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
	RNGItemData->ItemRarity = RarityRNG;
	float DamageRNG = FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);;
	float DefenseRNG = FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);;
	float HealthRestoreRNG = FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);;
	float CriticalRNG = FMath::FRandRange(0.75f + RarityRNG * 0.25f, 0.75f + (RarityRNG + 1) * 0.25f);;
	float ValueRNG = 0.f;
	ValueRNG += RNGItemData->Damage > 0.f ? DamageRNG : 1.f;
	ValueRNG += RNGItemData->Defense > 0.f ? DefenseRNG : 1.f;
	ValueRNG += RNGItemData->HealthToRestore > 0.f ? HealthRestoreRNG : 1.f;
	ValueRNG += RNGItemData->CriticalRate > 0.f ? CriticalRNG : 1.f;
	RNGItemData->Value *= ValueRNG / 4.f;
	RNGItemData->Damage *= DamageRNG;
	RNGItemData->Defense *= DefenseRNG;
	RNGItemData->HealthToRestore *= HealthRestoreRNG;
	RNGItemData->CriticalRate *= CriticalRNG;
}

void UCSubsystem_ItemManager::ReleaseItemInstance(UCItemData* ItemInstanceToRelease)
{
	if (!ItemInstanceToRelease)
	{
		return;
	}
	if (InstancedItemCache.Find(FIntPoint(ItemInstanceToRelease->ItemCategory, ItemInstanceToRelease->ItemCode)) == nullptr) return;
	TArray<TObjectPtr<UCItemData>>* tempArr = nullptr;
	tempArr = InstancedItemCache.Find(FIntPoint(ItemInstanceToRelease->ItemCategory, ItemInstanceToRelease->ItemCode));
	for (auto It : *tempArr)
	{
		if (It == ItemInstanceToRelease)
		{
			//UE_LOG(LogTemp, Log, TEXT("Released cached instance for item (Category: %d, Code: %d)."), It.Key().X, It.Key().Y);
			tempArr->Remove(It);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("ReleaseItemInstance: Could not find the specified instance in the cache."));
}
