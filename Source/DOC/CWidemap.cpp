#include "CWidemap.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Styling/SlateBrush.h"
#include "Materials/MaterialInstanceDynamic.h"

UCWidemap::UCWidemap(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInstance> WidemapFinder(TEXT("/Game/UI/MI_Widemap"));
	if (WidemapFinder.Succeeded()) MI_Widemap = WidemapFinder.Object;
}

bool UCWidemap::Initialize()
{
	bool rtn = Super::Initialize();
	Image_Widemap->SetBrushSize(FVector2D(800.f, 800.f));
	WidemapRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 800, 800);
	WidemapRenderTarget->ClearColor = FLinearColor::Transparent;
	if (MI_Widemap != nullptr) MID_Widemap = UMaterialInstanceDynamic::Create(MI_Widemap, this);
	if (MID_Widemap != nullptr && WidemapRenderTarget != nullptr) MID_Widemap->SetTextureParameterValue("Widemap", WidemapRenderTarget);
	if (MID_Widemap != nullptr && Image_Widemap != nullptr) Image_Widemap->Brush.SetResourceObject(MID_Widemap);
	return rtn;
}

void UCWidemap::SetMinimapAngle(float Angle)
{
	if (Image_Widemap != nullptr) Image_Widemap->SetRenderTransformAngle(Angle);
}
