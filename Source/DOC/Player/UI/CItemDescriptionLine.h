#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PCH.h"
#include "CItemDescriptionLine.generated.h"

UCLASS(BlueprintType)
class DOC_API UCItemDescriptionLine : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Content")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Content")
	FText CompareDescription;

	UPROPERTY(BlueprintReadOnly, Category = "Content")
	int32 DescriptionType;

	UPROPERTY(BlueprintReadOnly, Category = "Content")
	float ComparedValue;

	void Initialize(const FText& InDescription, const FText& InCompareDescription, int32 InDescriptionType, float InComparedValue = 0.0f)
	{
		Description = InDescription;
		CompareDescription = InCompareDescription;
		DescriptionType = InDescriptionType;
		ComparedValue = InComparedValue;
	}
};