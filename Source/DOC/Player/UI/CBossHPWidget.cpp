#include "Player/UI/CBossHPWidget.h"

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
	Super::UpdateBar(MonsterStat);
	if (MonsterStat.Groggy >= MonsterStat.MaxGroggy) SetGroggyPanelVisibility(true);
}

void UCBossHPWidget::OnGroggyEnd()
{
	SetGroggyPanelVisibility(false);
}