#include "Player/UI/CChestItemWidget.h"
#include "Components/Tileview.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IUIInventoryItem.h"
#include "PCH.h"

bool UCChestItemWidget::Initialize()
{
	bool rtn = Super::Initialize();

	if (ItemList != nullptr)
	{
		ItemList->OnEntryWidgetReleased().AddLambda([this](UUserWidget& Widget)
			{
				IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(&Widget);
				if (UIInventoryItem != nullptr)
				{
					UIInventoryItem->SetDelegates(nullptr, nullptr, nullptr);
				}
			});
		ItemList->OnEntryWidgetGenerated().AddLambda([this](UUserWidget& Widget)
			{
				IIUIInventoryItem* UIInventoryItem = Cast<IIUIInventoryItem>(&Widget);
				if (UIInventoryItem != nullptr)
				{
					UIInventoryItem->SetDelegates(ItemHoveredDelegatePtr, ItemUnHoveredDelegatePtr, GetItemDelegatePtr);
				}
			});
	}

    if (BackgroundBorder != nullptr)
    {
        DefaultBorderColor = BackgroundBorder->GetBrushColor();
    }
	if (AroundBorder != nullptr)
	{
		AroundBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (GetItemButton != nullptr)
	{
		GetItemButton->OnClicked.AddDynamic(this, &UCChestItemWidget::OnGetItemButtonClicked);
		GetItemButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	return rtn;
}

void UCChestItemWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TransitionProgress >= FadeOutDuration) return;

	TransitionProgress += InDeltaTime;

	if (TransitionProgress < 0.f)
	{
        float Alpha = 1.0f + (TransitionProgress / FadeInDuration);
        FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(DefaultBorderColor, TargetColor, Alpha);
        if (BackgroundBorder)
        {
            BackgroundBorder->SetBrushColor(CurrentColor);
        }
	}
	else if (TransitionProgress < FadeOutDuration)
	{
		float Alpha = (TransitionProgress / FadeOutDuration);
		FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(TargetColor, DefaultBorderColor, Alpha);
		if (BackgroundBorder)
		{
			BackgroundBorder->SetBrushColor(CurrentColor);
		}
	}
	else
	{
		TransitionProgress = FadeOutDuration;
		BackgroundBorder->SetBrushColor(DefaultBorderColor);
	}

}

void UCChestItemWidget::OpenChest(TArray<UCItemData*>* ToShowItemData)
{
	if (AroundBorder != nullptr) AroundBorder->SetVisibility(ESlateVisibility::Visible);
	if (GetItemButton != nullptr) GetItemButton->SetVisibility(ESlateVisibility::Collapsed);
	ChestItemTopRarity = 1;
	ItemList->ClearListItems();
	DisplayItemDataArr = ToShowItemData;
	DisplayItemIndex = 0;
	SetVisibility(ESlateVisibility::Visible);
	if (DisplayItemDataArr == nullptr || DisplayItemDataArr->Num() < 1) return;
	GetWorld()->GetTimerManager().ClearTimer(DisplayTimerHandle);
	ChestItemShowIter();
}

void UCChestItemWidget::SetDelegates(FOnItemHovered* InItemHoveredDelegatePtr, FOnItemUnhovered* InItemUnHoveredDelegatePtr, FGETITEM* InGetItemDelegatePtr)
{
	ItemHoveredDelegatePtr = InItemHoveredDelegatePtr;
	ItemUnHoveredDelegatePtr = InItemUnHoveredDelegatePtr;
	GetItemDelegatePtr = InGetItemDelegatePtr;
	if (GetItemDelegatePtr != nullptr)
	{
		GetItemDelegatePtr->AddUFunction(this, TEXT("OnGetItem"));
	}
}

void UCChestItemWidget::SetKeyboardDelegate(FPressedKeyboard* InPressedKeyboardDelegatePtr)
{
	PressedKeyboardDelegatePtr = InPressedKeyboardDelegatePtr;
	if (PressedKeyboardDelegatePtr != nullptr)
	{
		PressedKeyboardDelegatePtr->AddUFunction(this, TEXT("OnPressedKeyboard"));
	}
}

void UCChestItemWidget::ChestItemShowIter()
{
	if (DisplayItemDataArr != nullptr && DisplayItemDataArr->IsValidIndex(DisplayItemIndex))
	{
        UCItemData* CurrentItem = (*DisplayItemDataArr)[DisplayItemIndex];
        if (CurrentItem == nullptr) return;

		ItemList->AddItem(CurrentItem);

		SourceColor = DefaultBorderColor;
		switch (CurrentItem->ItemRarity)
		{
		case(ITEM_RARITY_NORMAL):
			TargetColor = ITEM_COLOR_NORMAL;
			break;
		case(ITEM_RARITY_RARE):
			TargetColor = ITEM_COLOR_RARE;
			break;
		case(ITEM_RARITY_EPIC):
			TargetColor = ITEM_COLOR_EPIC;
			break;
		case(ITEM_RARITY_LEGENDARY):
			TargetColor = ITEM_COLOR_LEGENDARY;
			break;
		}

		if (ChestItemTopRarity < CurrentItem->ItemRarity)
		{
			ChestItemTopRarity = CurrentItem->ItemRarity;
			AroundBorder->SetBrushColor(TargetColor);
		}
		if (CurrentItem->ItemRarity > 1)
		{
			FadeInDuration = CurrentItem->ItemRarity * 0.4f;
			FadeOutDuration = CurrentItem->ItemRarity * 0.1f;
			TransitionProgress = -FadeInDuration;
		}

		DisplayItemIndex++;
		if (DisplayItemDataArr->IsValidIndex(DisplayItemIndex))
		{
			GetWorld()->GetTimerManager().SetTimer(DisplayTimerHandle, this, &UCChestItemWidget::ChestItemShowIter, 0.15f + CurrentItem->ItemRarity * 0.05f, false);
		}
		else 
		{
			GetItemButton->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		GetItemButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UCChestItemWidget::OnGetItemButtonClicked()
{
	for (UCItemData* ItemData : *DisplayItemDataArr)
	{
		GetItemDelegatePtr->Broadcast(ItemData);
	}
}

void UCChestItemWidget::OnGetItem(UCItemData* InItemData)
{
	if (GetVisibility() != ESlateVisibility::Visible) return;
	if (InItemData == nullptr) return;
	if (ItemList == nullptr) return;
	ItemList->RemoveItem(InItemData);
	DisplayItemDataArr->Remove(InItemData);
}

void UCChestItemWidget::OnPressedKeyboard(FKey Key)
{
	if (Key == EKeys::E && GetItemButton->GetVisibility() == ESlateVisibility::Visible)
	{
		OnGetItemButtonClicked();
	}
}
