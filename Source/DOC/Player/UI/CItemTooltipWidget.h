#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
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

	void SetItemData(class UCItemData* InItemData);
};