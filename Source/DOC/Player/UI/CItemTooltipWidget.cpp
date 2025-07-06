#include "Player/UI/CItemTooltipWidget.h"
#include "Player/UI/CItemData.h"

void UCItemTooltipWidget::SetItemData(UCItemData* InItemData)
{
	if (InItemData == nullptr) return;

	if (ItemNameText != nullptr)
	{
		ItemNameText->SetText(FText::FromName(InItemData->ItemName));
	}
	if (ItemDescriptionText != nullptr)
	{
		// ItemData에 Description 필드가 있다고 가정합니다.
		// 만약 없다면, 이 부분을 InItemData->ItemName.ToString() 등으로 대체하거나, ItemData 클래스에 Description 필드를 추가해야 합니다.
		ItemDescriptionText->SetText(FText::FromString(TEXT("Item Description Placeholder"))); // 임시 텍스트
	}
}