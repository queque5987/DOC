#include "CHUD.h"
#include "Interfaces/IStageBuild.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Styling/SlateBrush.h"

UCHUD::UCHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UCHUD::SetupParameterDelegates(FHP_CHANGED* Delegate_HPChanged)
{
	Delegate_HPChanged->BindLambda(
		[&](float MaxHP, float CurrHP)
		{
			TEXT_HP->SetText(FText::FromString(FString::Printf(TEXT("%f / %f"), CurrHP, MaxHP)));
			HPBar->SetPercent(CurrHP / MaxHP);
		}
	);
}

bool UCHUD::Initialize()
{
	bool rtn = Super::Initialize();
	Image_Minimap->SetBrushSize(FVector2D(300.f, 300.f));
	MinimapRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 300, 300);
	if (Image_Minimap != nullptr && MinimapRenderTarget != nullptr) Image_Minimap->Brush.SetResourceObject(MinimapRenderTarget);
	return rtn;
}

void UCHUD::SetMinimapAngle(float Angle)
{
	if (Image_Minimap != nullptr) Image_Minimap->SetRenderTransformAngle(Angle);
}
