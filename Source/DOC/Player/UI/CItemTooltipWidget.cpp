#include "Player/UI/CItemTooltipWidget.h"
#include "Player/UI/CItemData.h"
#include "Components/Image.h"
#include "PCH.h" // PCH.h를 포함하여 등급별 색상 상수를 사용합니다.

void UCItemTooltipWidget::SetItemData(UCItemData* InItemData)
{
	if (InItemData == nullptr) return;

	if (ItemNameText != nullptr)
	{
		ItemNameText->SetText(FText::FromName(InItemData->ItemName));

		FLinearColor RarityColor;
		switch (InItemData->ItemRarity)
		{
		case ITEM_RARITY_NORMAL:
			RarityColor = ITEM_COLOR_NORMAL;
			break;
		case ITEM_RARITY_RARE:
			RarityColor = ITEM_COLOR_RARE;
			break;
		case ITEM_RARITY_EPIC:
			RarityColor = ITEM_COLOR_EPIC;
			break;
		case ITEM_RARITY_LEGENDARY:
			RarityColor = ITEM_COLOR_LEGENDARY;
			break;
		default:
			RarityColor = FLinearColor::White;
			break;
		}
		ItemNameText->SetColorAndOpacity(RarityColor);
	}

	if (ItemDescriptionText != nullptr)
	{
		ItemDescriptionText->SetText(InItemData->ItemDescription);
	}

	if (TotalAmountText != nullptr)
	{
		TotalAmountText->SetText(FText::Format(NSLOCTEXT("ItemTooltip", "TotalAmountFormat", "Amount: {0}"), InItemData->ItemCount));
	}

	if (TotalValueText != nullptr)
	{
		TotalValueText->SetText(FText::Format(NSLOCTEXT("ItemTooltip", "TotalValueFormat", "Value: {0}"), InItemData->Value));
	}

	if (ItemIconImage != nullptr && InItemData->ItemIcon != nullptr)
	{
		ItemIconImage->SetBrushFromTexture(InItemData->ItemIcon);
	}
}
