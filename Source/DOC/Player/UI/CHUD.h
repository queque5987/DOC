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
	TObjectPtr<UProgressBar> GroggyBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> HitEffect;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TEXT_HP;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TEXT_MP;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DieText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> Quickslot_1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> Quickslot_2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> Quickslot_3;
	TArray<TObjectPtr<class UTileView>> Quickslots;

private:
	const FName Param_Frame = FName("Frame");
	const FName Param_MaxFrame = FName("MaxFrame");
	const FName Param_MidFrame = FName("MidFrame");
	const FName Param_SubFrame = FName("SubFrame");

	const float HitEffectMaxFrame = 7.f;
	const float HitEffectMidFrame = 0.75f;
	const float HitEffectSubFrame = 0.25f;
	float HitEffectCurrentFrame = 0.f;

	UPROPERTY()
	float CurrentHPPercent;
	UPROPERTY()
	float TargetHPPercent;
	UPROPERTY()
	float CurrentMPPercent;
	UPROPERTY()
	float TargetMPPercent;
	UPROPERTY()
	float CurrentGroggyPercent;
	UPROPERTY()
	float TargetGroggyPercent;

	UPROPERTY(EditAnywhere, Category = "Config")
	float BarInterpSpeed = 5.f;

protected:
	class UCanvasRenderTarget2D* MinimapRenderTarget;
	TArray<class UCItemData*> QuickslotItemsArr;
	UPROPERTY()
	TObjectPtr<class UMaterialInstance> HitScreenMaterial;
	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> HitScreenMID;

	bool bDead = false;
public:
	UFUNCTION()
	void OnQuickslotChangedFunc(const TArray<class UCItemData*>& QuickslotItems);

	FOnQuickslotChanged OnQuickslotChanged;
	//FSlateBrush SlateBrush;
	void SetupParameterDelegates(FOnStatusChanged* Delegate_StatusChanged);

	void OnReceiveDamage() { HitEffectCurrentFrame = 0.f; };
	void OnDeath();

	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void SetMinimapAngle(float Angle) override;
	virtual class UCanvasRenderTarget2D* GetMinimapRT2D() override { return MinimapRenderTarget; };
	void ToggleMinimap(bool e) { if (Image_Minimap != nullptr) Image_Minimap->SetVisibility(e ? ESlateVisibility::Visible : ESlateVisibility::Collapsed); };
	class UCItemData* GetQuickslotItemData(int32 QuickslotIndex) { return QuickslotItemsArr.IsValidIndex(QuickslotIndex) ? QuickslotItemsArr[QuickslotIndex] : nullptr; };
};
