#include "Player/UI/CItemDescriptionLineWidget.h"
#include "Player/UI/CItemDescriptionLine.h"

void UCItemDescriptionLineWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UCItemDescriptionLine* LineData = Cast<UCItemDescriptionLine>(ListItemObject);
	if (LineData == nullptr) return;

	if (ItemStatusText != nullptr)
	{
		ItemStatusText->SetText(LineData->Description);
	}

	if (ItemStatusCompareText != nullptr)
	{
		ItemStatusCompareText->SetText(LineData->CompareDescription);

		FLinearColor ColorToSet = FLinearColor::White;
		if (LineData->ComparedValue > 0)
		{
			ColorToSet = FLinearColor::Green;
		}
		else if (LineData->ComparedValue < 0)
		{
			ColorToSet = FLinearColor::Red;
		}
		ItemStatusCompareText->SetColorAndOpacity(ColorToSet);
	}
}