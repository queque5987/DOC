#include "CHUD.h"
#include "Interfaces/IStageBuild.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Styling/SlateBrush.h"
#include "Components/Tileview.h"

UCHUD::UCHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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

	return rtn;
}

void UCHUD::SetMinimapAngle(float Angle)
{
	if (Image_Minimap != nullptr) Image_Minimap->SetRenderTransformAngle(Angle);
}

void UCHUD::OnQuickslotChangedFunc(const TArray<class UCItemData*>& QuickslotItems)
{
	if (Quickslot_1 != nullptr)
	{
		Quickslot_1->ClearListItems();
		if (QuickslotItems.IsValidIndex(0))
		{
			Quickslot_1->AddItem(QuickslotItems[0]);
		}
	}
	if (Quickslot_2 != nullptr)
	{
		Quickslot_2->ClearListItems();
		if (QuickslotItems.IsValidIndex(1))
		{
			Quickslot_2->AddItem(QuickslotItems[1]);
		}
	}
	if (Quickslot_3 != nullptr)
	{
		Quickslot_3->ClearListItems();
		if (QuickslotItems.IsValidIndex(2))
		{
			Quickslot_3->AddItem(QuickslotItems[2]);
		}
	}
}