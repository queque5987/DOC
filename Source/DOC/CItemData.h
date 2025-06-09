#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CItemData.generated.h"

UCLASS(BlueprintType)
class DOC_API UCItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	class UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemCode;

	void AddItemCount(int32 c = 1) { ItemCount += c; };
};
