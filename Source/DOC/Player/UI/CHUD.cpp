#include "CHUD.h"
#include "Interfaces/IStageBuild.h"
#include "Interfaces/IUIInventoryItem.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Styling/SlateBrush.h"
#include "Components/Tileview.h"

UCHUD::UCHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	QuickslotItemsArr.SetNum(3);
	CurrentHPPercent = 1.f;
	TargetHPPercent = 1.f;
	CurrentMPPercent = 1.f;
	TargetMPPercent = 1.f;
}

void UCHUD::SetupParameterDelegates(FOnStatusChanged* Delegate_StatusChanged)
{
	Delegate_StatusChanged->AddLambda(
		[&](FPlayerStat NewPlayerStat)
		{
			TEXT_HP->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), NewPlayerStat.CurrHP, NewPlayerStat.MaxHP)));
			TargetHPPercent = NewPlayerStat.MaxHP > 0 ? NewPlayerStat.CurrHP / NewPlayerStat.MaxHP : 0.f;

			TEXT_MP->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), NewPlayerStat.CurrMP, NewPlayerStat.MaxMP)));
			TargetMPPercent = NewPlayerStat.MaxMP > 0 ? NewPlayerStat.CurrMP / NewPlayerStat.MaxMP : 0.f;
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

void UCHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!FMath::IsNearlyEqual(CurrentHPPercent, TargetHPPercent))
	{
		CurrentHPPercent = FMath::FInterpTo(CurrentHPPercent, TargetHPPercent, InDeltaTime, BarInterpSpeed);
		HPBar->SetPercent(CurrentHPPercent);
	}

	if (!FMath::IsNearlyEqual(CurrentMPPercent, TargetMPPercent))
	{
		CurrentMPPercent = FMath::FInterpTo(CurrentMPPercent, TargetMPPercent, InDeltaTime, BarInterpSpeed);
		MPBar->SetPercent(CurrentMPPercent);
	}
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