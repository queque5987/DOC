#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PCH.h"
#include "CItemTooltipWidget.generated.h"

UCLASS()
class DOC_API UCItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDescriptionText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TotalAmountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TotalValueText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIconImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemRarityCorner; // New: Rarity Corner Image

	void SetItemData(class UCItemData* InItemData);
};
