#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interfaces/IHUD.h"
#include "Interfaces/CStageDelegateTypes.h"
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> Quickslot_1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> Quickslot_2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> Quickslot_3;
protected:
	class UCanvasRenderTarget2D* MinimapRenderTarget;
public:
	UFUNCTION()
	void OnQuickslotChangedFunc(const TArray<class UCItemData*>& QuickslotItems);

	FOnQuickslotChanged OnQuickslotChanged;
	//FSlateBrush SlateBrush;
	void SetupParameterDelegates(FOnStatusChanged* Delegate_StatusChanged);

	virtual bool Initialize() override;
	virtual void SetMinimapAngle(float Angle) override;
	virtual class UCanvasRenderTarget2D* GetMinimapRT2D() override { return MinimapRenderTarget; };
	void ToggleMinimap(bool e) { if (Image_Minimap != nullptr) Image_Minimap->SetVisibility(e ? ESlateVisibility::Visible : ESlateVisibility::Collapsed); };
};
