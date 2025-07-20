#include "Player/UI/CItemTooltipWidget.h"
#include "Player/UI/CItemData.h"

void UCItemTooltipWidget::SetItemData(UCItemData* InItemData)
{
	if (InItemData == nullptr) return;

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

	if (ItemNameText != nullptr)
	{
		ItemNameText->SetText(FText::FromName(InItemData->ItemName));
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

	if (ItemStatusText != nullptr)
	{
		FString StatusString;
		if (InItemData->HealthToRestore > 0) StatusString += FString::Printf(TEXT("- Health: +%.0f\n"), InItemData->HealthToRestore);
		if (InItemData->Damage > 0) StatusString += FString::Printf(TEXT("- Damage: +%.0f\n"), InItemData->Damage);
		if (InItemData->Defense > 0) StatusString += FString::Printf(TEXT("- Defense: +%.0f\n"), InItemData->Defense);
		ItemStatusText->SetText(FText::FromString(StatusString));
	}

	if (ItemIconImage != nullptr && InItemData->ItemIcon != nullptr)
	{
		ItemIconImage->SetBrushFromTexture(InItemData->ItemIcon);
	}

	if (ItemRarityCorner != nullptr)
	{
		ItemRarityCorner->SetColorAndOpacity(RarityColor);
	}
}

