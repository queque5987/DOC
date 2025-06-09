#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "IHUD.h"
#include "CHUD.generated.h"

UCLASS()
class DOC_API UCHUD : public UUserWidget, public IIHUD
{
	GENERATED_BODY()

	UCHUD(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Minimap;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> MPBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TEXT_HP;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TEXT_MP;
protected:
	class UCanvasRenderTarget2D* MinimapRenderTarget;
public:
	//FSlateBrush SlateBrush;
	virtual bool Initialize() override;
	virtual void SetMinimapAngle(float Angle) override;
	virtual class UCanvasRenderTarget2D* GetMinimapRT2D() override { return MinimapRenderTarget; };
	void ToggleMinimap(bool e) { if (Image_Minimap != nullptr) Image_Minimap->SetVisibility(e ? ESlateVisibility::Visible : ESlateVisibility::Collapsed); };
};
