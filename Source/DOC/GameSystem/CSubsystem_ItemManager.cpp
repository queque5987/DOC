#include "GameSystem/CSubsystem_ItemManager.h"
#include "Player/UI/CItemData.h"
#include "Engine/ObjectLibrary.h"

void UCSubsystem_ItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PopulateItemMap();
}

UCItemData* UCSubsystem_ItemManager::GetOrCreateItemInstance(int32 ItemCategory, int32 ItemCode)
{
	const FIntPoint ItemKey(ItemCategory, ItemCode);
	bool IsEquipment = ItemCategory == ITEM_CATEGORY_EQUIPMENT;

	UCItemData* BaseDataAsset = FindDataAsset(ItemCategory, ItemCode);
	if (BaseDataAsset)
	{
		UCItemData* NewInstance = DuplicateObject<UCItemData>(BaseDataAsset, this);
		if(NewInstance)
		{
			if (IsEquipment) RNGEquipmentStats(NewInstance);
			InstancedItemCache.FindOrAdd(ItemKey).Add(NewInstance);
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

void UCSubsystem_ItemManager::PopulateItemMap()
{
	CategorizedItemData.Empty();

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UCItemData::StaticClass(), true, GIsEditor);
	if (!ObjectLibrary)
	{
		UE_LOG(LogTemp, Error, TEXT("PopulateItemMap: Failed to create ObjectLibrary."));
		return;
	}

	const FString SearchPath = TEXT("/Game/Data/Item/DataAsset/");
	ObjectLibrary->LoadAssetDataFromPath(SearchPath);

	TArray<FAssetData> AssetDataList;
	ObjectLibrary->GetAssetDataList(AssetDataList);

	for (const FAssetData& AssetData : AssetDataList)
	{
		if (UCItemData* LoadedAsset = Cast<UCItemData>(AssetData.GetAsset()))
		{
			if (LoadedAsset->ItemCategory >= 0 && LoadedAsset->ItemCode >= 0) // Allow 0 as a valid ID
			{
				CategorizedItemData.FindOrAdd(LoadedAsset->ItemCategory).Add(LoadedAsset->ItemCode, LoadedAsset);
				UE_LOG(LogTemp, Log, TEXT("PopulateItemMap: Map populated %s"), LoadedAsset->ItemName);
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("PopulateItemMap: Map populated with %d categories."), CategorizedItemData.Num());
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
