#include "Player/UI/CBossHPWidget.h"

void UCBossHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GroggyBarOverride)
	{
		GroggyElipsedTime += InDeltaTime;
		float tempGroggyPercent = 1.f - GroggyElipsedTime / GroggyTotalDuration;
		GroggyBar->SetPercent(tempGroggyPercent);
	}
}

void UCBossHPWidget::SetupDelegate(FOnStatusChanged* InStatusChanged_DelegatePtr, FOnGroggyEnd* InGroggyEnd_DelegatePtr)
{
	if (StatusChanged_DelegatePtr != nullptr && StatusChanged_DelegateHandle.IsValid())
	{
		StatusChanged_DelegatePtr->Remove(StatusChanged_DelegateHandle);
		StatusChanged_DelegateHandle.Reset();
	}
	StatusChanged_DelegatePtr = InStatusChanged_DelegatePtr;
	if (StatusChanged_DelegatePtr != nullptr)
	{
		StatusChanged_DelegateHandle = StatusChanged_DelegatePtr->AddUFunction(this, TEXT("UpdateBar"));
	}

	if (OnGroggyEnd_DelegatePtr != nullptr && OnGroggyEnd_DelegateHandle.IsValid())
	{
		OnGroggyEnd_DelegatePtr->Remove(OnGroggyEnd_DelegateHandle);
		OnGroggyEnd_DelegateHandle.Reset();
	}
	OnGroggyEnd_DelegatePtr = InGroggyEnd_DelegatePtr;
	if (OnGroggyEnd_DelegatePtr != nullptr)
	{
		OnGroggyEnd_DelegateHandle = OnGroggyEnd_DelegatePtr->AddUFunction(this, TEXT("OnGroggyEnd"));
	}

}

void UCBossHPWidget::UpdateBar(FPlayerStat MonsterStat)
{
	TargetHealthPercent = MonsterStat.CurrHP / MonsterStat.MaxHP;
	TargetGroggyPercent = MonsterStat.Groggy / MonsterStat.MaxGroggy;
	if (HPBar)
	{
		HPBar->SetPercent(TargetHealthPercent);
		StopDelayInterpolation();
		if (GetWorld())
		{
			FTimerManager& TimerManager = GetWorld()->GetTimerManager();
			TimerManager.ClearTimer(HPDelayTimerHandle);
			TimerManager.SetTimer(HPDelayTimerHandle, this, &UCBossHPWidget::StartDelayInterpolation, 1.f, false);
		}
	}
	if (GroggyBar && !GroggyBarOverride)
	{
		GroggyBar->SetPercent(TargetGroggyPercent);
	}
	if (MonsterStat.Groggy >= MonsterStat.MaxGroggy && !GroggyBarOverride) SetGroggyPanelVisibility(true);
}

void UCBossHPWidget::SetGroggyPanelVisibility(bool e)
{
	Super::SetGroggyPanelVisibility(e);
	if (!GroggyBarOverride && e)
	{
		GroggyElipsedTime = 0.f;
		OverridenGroggyBarPercent = 1.f;
	}
	GroggyBarOverride = e;
}

void UCBossHPWidget::OnGroggyEnd()
{
	SetGroggyPanelVisibility(false);
}