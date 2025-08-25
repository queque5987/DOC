#include "Player/UI/CInventoryItem_E.h"
//#include "Player/UI/CItemData.h"
#include "Components/Image.h"

void UCInventoryItem_E::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	if (ItemData != nullptr && GetEffectBG != nullptr)
	{
		FLinearColor RarityColor;
		switch (ItemData->ItemRarity)
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
		GetEffectBG->SetColorAndOpacity(RarityColor);
		GetEffectBG->SetVisibility(
			ItemData->ItemRarity == ITEM_RARITY_NORMAL ? ESlateVisibility::Collapsed : ESlateVisibility::Visible
		);
	}
}
