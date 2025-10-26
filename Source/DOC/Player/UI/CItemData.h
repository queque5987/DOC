#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CItemData.generated.h"

UCLASS(BlueprintType)
class DOC_API UCItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UCItemData(const FObjectInitializer& ObjectInitializer);
	~UCItemData();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Display")
	class UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Display")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Display", meta = (MultiLine = true))
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Classification")
	int32 ItemCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Classification")
	int32 ItemCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Classification")
	int32 ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Classification")
	int32 ItemEquipSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Inventory")
	int32 ItemCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Inventory")
	bool bIsStackable = false;

	//float ItemQuality_Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particle")
	int32 ItemUseSpawnParticle = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stats")
	float ItemCoolDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stats")
	float HealthToRestore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stats")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stats")
	float Defense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stats")
	float CriticalRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | World")
	TSubclassOf<AActor> ItemActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Economy")
	int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | UI")
	TSubclassOf<class UUserWidget> DragVisualClass;

	bool Equipped = false;
	int32 Quickslot = -1;

	void AddItemCount(int32 c = 1) { ItemCount += c; };

	void Copy(UCItemData* ToCopyItemData);
};
