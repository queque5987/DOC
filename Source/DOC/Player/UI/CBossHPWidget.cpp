#include "Player/UI/CBossHPWidget.h"

void UCBossHPWidget::SetupDelegate(FOnStatusChanged* InStatusChanged_DelegatePtr)
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
}
