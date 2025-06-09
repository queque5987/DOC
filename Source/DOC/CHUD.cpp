#include "CHUD.h"
#include "IStageBuild.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Styling/SlateBrush.h"

UCHUD::UCHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool UCHUD::Initialize()
{
	bool rtn = Super::Initialize();
	Image_Minimap->SetBrushSize(FVector2D(300.f, 300.f));
	MinimapRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 300, 300);
	SlateBrush = FSlateBrush();
	SlateBrush.SetResourceObject(MinimapRenderTarget);
	SlateBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	if (Image_Minimap != nullptr && MinimapRenderTarget != nullptr) Image_Minimap->SetBrush(SlateBrush);
	return rtn;
}

void UCHUD::SetMinimapAngle(float Angle)
{
	if (Image_Minimap != nullptr) Image_Minimap->SetRenderTransformAngle(Angle);
}
