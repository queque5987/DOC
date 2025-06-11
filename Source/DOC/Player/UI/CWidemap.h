#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Interfaces/IHUD.h"
#include "CWidemap.generated.h"

UCLASS()
class DOC_API UCWidemap : public UUserWidget, public IIHUD
{
	GENERATED_BODY()
	UCWidemap(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Widemap;
protected:
	class UCanvasRenderTarget2D* WidemapRenderTarget;
	class UMaterialInstance* MI_Widemap;
	class UMaterialInstanceDynamic* MID_Widemap;
public:
	virtual bool Initialize() override;
	virtual class UCanvasRenderTarget2D* GetMinimapRT2D() override { return WidemapRenderTarget; };
	virtual void SetMinimapAngle(float Angle) override;
	
};
