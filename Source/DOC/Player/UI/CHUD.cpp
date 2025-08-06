#include "CHUD.h"
#include "Interfaces/IStageBuild.h"
#include "Interfaces/IUIInventoryItem.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Styling/SlateBrush.h"
#include "Components/Tileview.h"

UCHUD::UCHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	QuickslotItemsArr.SetNum(3);
}

void UCHUD::SetupParameterDelegates(FOnStatusChanged* Delegate_StatusChanged)
{
	Delegate_StatusChanged->AddLambda(
		[&](FPlayerStat NewPlayerStat)
		{
			TEXT_HP->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), NewPlayerStat.CurrHP, NewPlayerStat.MaxHP)));
			HPBar->SetPercent(NewPlayerStat.CurrHP / NewPlayerStat.MaxHP);
			MPBar->SetPercent(NewPlayerStat.CurrMP / NewPlayerStat.MaxMP);
		}
	);
}

bool UCHUD::Initialize()
{
	bool rtn = Super::Initialize();
	Image_Minimap->SetBrushSize(FVector2D(300.f, 300.f));
	MinimapRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 300, 300);
	if (Image_Minimap != nullptr && MinimapRenderTarget != nullptr) Image_Minimap->Brush.SetResourceObject(MinimapRenderTarget);

	OnQuickslotChanged.AddUFunction(this, FName("OnQuickslotChangedFunc"));

	Quickslots.Add(Quickslot_1);
	Quickslots.Add(Quickslot_2);
	Quickslots.Add(Quickslot_3);

	return rtn;
}

void UCHUD::SetMinimapAngle(float Angle)
{
	if (Image_Minimap != nullptr) Image_Minimap->SetRenderTransformAngle(Angle);
}

void UCHUD::OnQuickslotChangedFunc(const TArray<class UCItemData*>& QuickslotItems)
{
	for (int32 i = 0; i < Quickslots.Num(); i++)
	{
		if (Quickslots.IsValidIndex(i) && Quickslots[i] != nullptr)
		{
			Quickslots[i]->ClearListItems();
			if (QuickslotItems.IsValidIndex(i) && QuickslotItems[i] != nullptr)
			{
				Quickslots[i]->AddItem(QuickslotItems[i]);
				QuickslotItemsArr[i] = QuickslotItems[i];

				TArray<UUserWidget*> DisplayedEntryWidgets = Quickslots[i]->GetDisplayedEntryWidgets();
				if (DisplayedEntryWidgets.IsValidIndex(0))
				{
					IIUIInventoryItem* IWidget = Cast<IIUIInventoryItem>(DisplayedEntryWidgets[0]);
					if (IWidget != nullptr)
					{
						IWidget->RefreshUI();
					}
				}
			}
			else
			{
				QuickslotItemsArr[i] = nullptr;
			}
		}
	}
}