#include "Player/UI/CItemTooltipWidget.h"
#include "Player/UI/CItemData.h"
#include "Player/UI/CItemDescriptionLine.h"
#include "Components/ListView.h"

void UCItemTooltipWidget::SetItemData(UCItemData* InItemData, UCItemData* CompairItemData)
{
	if (InItemData == nullptr && !InItemData->IsValidLowLevel()) return;

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

	if (TotalAmountText != nullptr)
	{
		TotalAmountText->SetText(FText::Format(NSLOCTEXT("ItemTooltip", "TotalAmountFormat", "Amount: {0}"), InItemData->ItemCount));
	}

	if (TotalValueText != nullptr)
	{
		TotalValueText->SetText(FText::Format(NSLOCTEXT("ItemTooltip", "TotalValueFormat", "Value: {0}"), InItemData->Value));
	}

	if (ItemDescriptionText != nullptr)
	{
		ItemDescriptionText->SetText(InItemData->ItemDescription);
	}

	// Status
	if (DescriptionListView != nullptr)
	{
		DescriptionListView->ClearListItems();
		if (InItemData->HealthToRestore > 0.f || (CompairItemData != nullptr && CompairItemData->HealthToRestore > 0.f))
		{
			FText Desc = FText::Format(NSLOCTEXT("ItemTooltip", "HealthFormat", "Health: +{0:.1f}"), InItemData->HealthToRestore);
			FText Comp = FText::GetEmpty();
			float Diff = 0.0f;
			if (CompairItemData != nullptr)
			{
				Diff = InItemData->HealthToRestore - CompairItemData->HealthToRestore;
				Comp = FText::Format(NSLOCTEXT("ItemTooltip", "CompFormat", "({0}{1})"), FText::FromString(Diff >= 0 ? TEXT("+") : TEXT("")), Diff);
			}
			UCItemDescriptionLine* NewLine = NewObject<UCItemDescriptionLine>(this);
			NewLine->Initialize(Desc, Comp, ITEM_DESCRIPTION_HEALTH_RESTORE, Diff);
			DescriptionListView->AddItem(NewLine);
		}
		if (InItemData->Damage > 0.f || (CompairItemData != nullptr && CompairItemData->Damage > 0.f))
		{
			FText Desc = FText::Format(NSLOCTEXT("ItemTooltip", "DamageFormat", "Damage: +{0:.1f}"), InItemData->Damage);
			FText Comp = FText::GetEmpty();
			float Diff = 0.0f;
			if (CompairItemData != nullptr)
			{
				Diff = InItemData->Damage - CompairItemData->Damage;
				Comp = FText::Format(NSLOCTEXT("ItemTooltip", "CompFormat", "({0}{1})"), FText::FromString(Diff >= 0 ? TEXT("+") : TEXT("")), Diff);
			}
			UCItemDescriptionLine* NewLine = NewObject<UCItemDescriptionLine>(this);
			NewLine->Initialize(Desc, Comp, ITEM_DESCRIPTION_DAMAGE, Diff);
			DescriptionListView->AddItem(NewLine);
		}
		if (InItemData->Defense > 0.f || (CompairItemData != nullptr && CompairItemData->Defense > 0.f))
		{
			FText Desc = FText::Format(NSLOCTEXT("ItemTooltip", "DefenseFormat", "Defense: +{0:.1f}"), InItemData->Defense);
			FText Comp = FText::GetEmpty();
			float Diff = 0.0f;
			if (CompairItemData != nullptr)
			{
				Diff = InItemData->Defense - CompairItemData->Defense;
				Comp = FText::Format(NSLOCTEXT("ItemTooltip", "CompFormat", "({0}{1})"), FText::FromString(Diff >= 0 ? TEXT("+") : TEXT("")), Diff);
			}
			UCItemDescriptionLine* NewLine = NewObject<UCItemDescriptionLine>(this);
			NewLine->Initialize(Desc, Comp, ITEM_DESCRIPTION_DEFENCE, Diff);
			DescriptionListView->AddItem(NewLine);
		}
		if (InItemData->CriticalRate > 0.f || (CompairItemData != nullptr && CompairItemData->CriticalRate > 0.f))
		{
			FText Desc = FText::Format(NSLOCTEXT("ItemTooltip", "CriticalRateFormat", "CriticalRate: +{0:.1f} %"), InItemData->CriticalRate);
			FText Comp = FText::GetEmpty();
			float Diff = 0.0f;
			if (CompairItemData != nullptr)
			{
				Diff = InItemData->CriticalRate - CompairItemData->CriticalRate;
				Comp = FText::Format(NSLOCTEXT("ItemTooltip", "CompFormat", "({0}{1})"), FText::FromString(Diff >= 0 ? TEXT("+") : TEXT("")), Diff);
			}
			UCItemDescriptionLine* NewLine = NewObject<UCItemDescriptionLine>(this);
			NewLine->Initialize(Desc, Comp, ITEM_DESCRIPTION_DEFENCE, Diff);
			DescriptionListView->AddItem(NewLine);
		}
	}

	if (ItemIconImage != nullptr && InItemData->ItemIcon != nullptr)
	{
		ItemIconImage->SetBrushFromTexture(InItemData->ItemIcon);
	}

	if (ItemRarityCorner != nullptr)
	{
		ItemRarityCorner->SetColorAndOpacity(RarityColor);
	}

	if (EquipedPanel != nullptr)
	{
		EquipedPanel->SetVisibility(InItemData->Equipped ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

