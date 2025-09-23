#include "Player/UI/CMonsterHPWidget.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

void UCMonsterHPWidget::UpdateHPBar(float CurrentHP, float MaxHP)
{
	TargetHealthPercent = CurrentHP / MaxHP;

	if (HPBar)
	{
		HPBar->SetPercent(TargetHealthPercent);
		StopDelayInterpolation();
	}
}

void UCMonsterHPWidget::UpdateBar(FPlayerStat MonsterStat)
{
	TargetHealthPercent = MonsterStat.CurrHP / MonsterStat.MaxHP;
	TargetGroggyPercent = MonsterStat.Groggy / MonsterStat.MaxGroggy;
	if (HPBar)
	{
		HPBar->SetPercent(TargetHealthPercent);
		StopDelayInterpolation();
	}
	if (GroggyBar)
	{
		GroggyBar->SetPercent(TargetGroggyPercent);
	}
}

void UCMonsterHPWidget::SetDelayHP(float NewDelayHPPercent)
{
	if (HPBar_Delay)
	{
		HPBar_Delay->SetPercent(NewDelayHPPercent);
		CurrentDelayHealthPercent = NewDelayHPPercent;
	}
	StopDelayInterpolation();
}

void UCMonsterHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TargetHealthPercent = 1.0f;
	CurrentDelayHealthPercent = 1.0f;
	bIsDelaying = false;

	if (HPBar) HPBar->SetPercent(1.0f);
	if (HPBar_Delay) HPBar_Delay->SetPercent(1.0f);
	if (GroggyBar) GroggyBar->SetPercent(0.f);
	if (CounterPannel) CounterPannel->SetVisibility(ESlateVisibility::Collapsed);
}

void UCMonsterHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsDelaying && HPBar_Delay)
	{
		CurrentDelayHealthPercent = FMath::FInterpTo(CurrentDelayHealthPercent, TargetHealthPercent, InDeltaTime, DelayInterpSpeed);
		HPBar_Delay->SetPercent(CurrentDelayHealthPercent);

		if (FMath::IsNearlyEqual(CurrentDelayHealthPercent, TargetHealthPercent, 0.001f))
		{
			StopDelayInterpolation();
		}
	}
}

void UCMonsterHPWidget::StartDelayInterpolation()
{
	bIsDelaying = true;
	if (HPBar_Delay)
	{
		CurrentDelayHealthPercent = HPBar_Delay->GetPercent();
	}
}

void UCMonsterHPWidget::StopDelayInterpolation()
{
	bIsDelaying = false;
}

void UCMonsterHPWidget::SetGroggyExecuteCountAllOutDelegate(FOnGroggyExecuteCountAllOut* InOnGroggyExecuteCountAllOut_DelegatePtr)
{
	if (OnGroggyExecuteCountAllOut_DelegatePtr != nullptr && GroggyExecuteCountAllOut_DelegateHandle.IsValid())
	{
		OnGroggyExecuteCountAllOut_DelegatePtr->Remove(GroggyExecuteCountAllOut_DelegateHandle);
	}
	OnGroggyExecuteCountAllOut_DelegatePtr = InOnGroggyExecuteCountAllOut_DelegatePtr;
	if (OnGroggyExecuteCountAllOut_DelegatePtr != nullptr)
	{
		OnGroggyExecuteCountAllOut_DelegatePtr->AddUFunction(this, TEXT("ShutdownGroggyPanel"));
	}
}

void UCMonsterHPWidget::SetGroggyPanelVisibility(bool e)
{
	if (CounterPannel) CounterPannel->SetVisibility(e ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UCMonsterHPWidget::ShutdownGroggyPanel()
{
	if (CounterPannel) CounterPannel->SetVisibility(ESlateVisibility::Collapsed);
}
