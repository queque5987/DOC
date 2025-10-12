#include "CHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/IStageBuild.h"
#include "Interfaces/IUIInventoryItem.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Styling/SlateBrush.h"
#include "Components/Tileview.h"
#include "Components/Border.h"
#include "Materials/MaterialInstanceDynamic.h"

UCHUD::UCHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> HitScreenMaterialFinder(TEXT("/Game/InventoryKit/Widgets/Common/Materials/MI_PlayerHitScreen.MI_PlayerHitScreen"));
	if (HitScreenMaterialFinder.Succeeded())
	{
		HitScreenMaterial = HitScreenMaterialFinder.Object;
	}

	QuickslotItemsArr.SetNum(3);
	CurrentHPPercent = 1.f;
	TargetHPPercent = 1.f;
	CurrentMPPercent = 1.f;
	TargetMPPercent = 1.f;
	CurrentGroggyPercent = 0.f;
	TargetGroggyPercent = 0.f;
}

void UCHUD::SetupParameterDelegates(FOnStatusChanged* Delegate_StatusChanged)
{
}

void UCHUD::SetupDelegates(FOnStatusChanged* Delegate_StatusChanged, FPressedKeyboard* Delegate_PressedKeyboard, FOnRevive* Delegate_OnRevive)
{
	if (Delegate_StatusChanged != nullptr)
	{
		Delegate_StatusChanged->AddLambda(
			[&](FPlayerStat NewPlayerStat)
			{
				TEXT_HP->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), NewPlayerStat.CurrHP, NewPlayerStat.MaxHP)));
				TargetHPPercent = NewPlayerStat.MaxHP > 0 ? NewPlayerStat.CurrHP / NewPlayerStat.MaxHP : 0.f;

				TEXT_MP->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), NewPlayerStat.CurrMP, NewPlayerStat.MaxMP)));
				TargetMPPercent = NewPlayerStat.MaxMP > 0 ? NewPlayerStat.CurrMP / NewPlayerStat.MaxMP : 0.f;

				TargetGroggyPercent = FMath::Clamp(NewPlayerStat.Groggy / NewPlayerStat.MaxGroggy, 0.f, 1.f);
			}
		);
	}
	if (Delegate_PressedKeyboard != nullptr) Delegate_PressedKeyboard->AddUFunction(this, TEXT("PreseedKeyboardButton"));
	if (Delegate_OnRevive != nullptr) OnRevive_DelegatePtr = Delegate_OnRevive;
}

void UCHUD::OnDeath()
{
	bDead = true;
	HitEffectCurrentFrame = 0.f;
	if (DieText != nullptr) DieText->SetVisibility(ESlateVisibility::Visible);
}

bool UCHUD::Initialize()
{
	bool rtn = Super::Initialize();
	Image_Minimap->SetBrushSize(FVector2D(300.f, 300.f));
	MinimapRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 300, 300);
	if (Image_Minimap != nullptr && MinimapRenderTarget != nullptr) Image_Minimap->Brush.SetResourceObject(MinimapRenderTarget);

	if (HitEffect != nullptr && HitScreenMaterial != nullptr)
	{
		HitScreenMID = UMaterialInstanceDynamic::Create(HitScreenMaterial, this);
		HitEffect->SetBrushFromMaterial(HitScreenMID);

		if (HitScreenMID != nullptr)
		{
			HitEffectCurrentFrame = HitEffectMaxFrame;
			HitScreenMID->SetScalarParameterValue(Param_Frame, HitEffectCurrentFrame);
			HitScreenMID->SetScalarParameterValue(Param_MaxFrame, HitEffectMaxFrame);
			HitScreenMID->SetScalarParameterValue(Param_MidFrame, HitEffectMidFrame);
			HitScreenMID->SetScalarParameterValue(Param_SubFrame, HitEffectSubFrame);
		}
	}

	OnQuickslotChanged.AddUFunction(this, FName("OnQuickslotChangedFunc"));

	Quickslots.Add(Quickslot_1);
	Quickslots.Add(Quickslot_2);
	Quickslots.Add(Quickslot_3);
	if (GroggyBar != nullptr) GroggyBar->SetPercent(0.f);
	if (DieText != nullptr)
	{
		DieText->SetVisibility(ESlateVisibility::Hidden);
		DieText->SetRenderOpacity(0.f);
	}
	if (ReviveText != nullptr)
	{
		ReviveText->SetVisibility(ESlateVisibility::Hidden);
		ReviveText->SetRenderOpacity(0.f);
	}
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

	if (!FMath::IsNearlyEqual(CurrentGroggyPercent, TargetGroggyPercent))
	{
		CurrentGroggyPercent = FMath::FInterpTo(CurrentGroggyPercent, TargetGroggyPercent, InDeltaTime, BarInterpSpeed);
		GroggyBar->SetPercent(CurrentGroggyPercent);
	}

	if (HitScreenMID != nullptr && HitEffectCurrentFrame < HitEffectMaxFrame)
	{
		HitEffectCurrentFrame += InDeltaTime;
		HitEffectCurrentFrame = FMath::Min(HitEffectCurrentFrame, HitEffectMaxFrame);
		HitScreenMID->SetScalarParameterValue(Param_Frame,
			bDead ? 
			FMath::Min(HitEffectCurrentFrame / HitEffectMaxFrame * 1.75f, 1.f) * HitEffectSubFrame :
			HitEffectCurrentFrame
		);
		if (bDead && !bReviveReady)
		{
			DieText->SetRenderOpacity(FMath::Min(HitEffectCurrentFrame / HitEffectMaxFrame, 1.f));
			if (HitEffectCurrentFrame / HitEffectMaxFrame > 0.8f)
			{
				if (ReviveText->GetVisibility() != ESlateVisibility::Visible) ReviveText->SetVisibility(ESlateVisibility::Visible);
				ReviveText->SetRenderOpacity(FMath::Min((HitEffectCurrentFrame / HitEffectMaxFrame - 0.8f) / 0.2f, 1.f));
			}
			if (HitEffectCurrentFrame >= HitEffectMaxFrame) bReviveReady = true;
		}
	}
}

void UCHUD::SetMinimapAngle(float Angle)
{
	if (Image_Minimap != nullptr) Image_Minimap->SetRenderTransformAngle(Angle);
}

void UCHUD::PreseedKeyboardButton(FKey PressedKey)
{
	if (PressedKey != EKeys::AnyKey) return;
	if (!bReviveReady) return;
	if (OnRevive_DelegatePtr == nullptr) return;
	OnRevive_DelegatePtr->Broadcast();
	bDead = false;
	bReviveReady = false;
	HitEffectCurrentFrame = HitEffectMaxFrame;
	HitScreenMID->SetScalarParameterValue(Param_Frame, HitEffectCurrentFrame);
	if (DieText != nullptr) DieText->SetVisibility(ESlateVisibility::Hidden);
	if (ReviveText != nullptr) ReviveText->SetVisibility(ESlateVisibility::Hidden);

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
			Quickslots[i]->RequestRefresh();
		}
	}
}