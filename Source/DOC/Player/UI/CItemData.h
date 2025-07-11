#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CItemData.generated.h"

UCLASS(BlueprintType)
class DOC_API UCItemData : public UDataAsset
{
	GENERATED_BODY()

public:
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Inventory")
	int32 ItemCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Inventory")
	bool bIsStackable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stats")
	float HealthToRestore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stats")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Stats")
	float Defense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | World")
	TSubclassOf<AActor> ItemActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Economy")
	int32 Value;

	void AddItemCount(int32 c = 1) { ItemCount += c; };
};
