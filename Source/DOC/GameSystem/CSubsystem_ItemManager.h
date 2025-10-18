#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PCH.h"
#include "CSubsystem_ItemManager.generated.h"

class UCItemData;

UCLASS()
class DOC_API UCSubsystem_ItemManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	UCSubsystem_ItemManager();
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Item Management")
	UCItemData* GetOrCreateItemInstance(int32 ItemCategory, int32 ItemCode);

	UFUNCTION(BlueprintCallable, Category = "Item Management")
	void ReleaseItemInstance(UCItemData* ItemInstanceToRelease);

private:
	TMap<int32, TMap<int32, TObjectPtr<UCItemData>>> CategorizedItemData;
	TMap<FIntPoint, TArray<TObjectPtr<UCItemData>>> InstancedItemCache;


	UCItemData* FindDataAsset(int32 ItemCategory, int32 ItemCode) const;
	//void PopulateItemMap();
	void RNGEquipmentStats(UCItemData*& RNGItemData);
};

