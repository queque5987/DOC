#include "Player/UI/CStageClearItemWidget.h"
#include "Components/ListView.h"
#include "Components/Border.h"
#include "Player/UI/CSpawnedEnemyData.h"
#include "TimerManager.h"

void UCStageClearItemWidget::SetupDelegates(FPressedKeyboard* InOnPressedKeyboardDelegatePtr, FGETITEM* InOnGetItemDelegatePtr)
{
	OnPressedKeyboardDelegatePtr = InOnPressedKeyboardDelegatePtr;
	OnGetItemDelegatePtr = InOnGetItemDelegatePtr;
	if (OnPressedKeyboardDelegatePtr != nullptr) OnPressedKeyboardDelegatePtr->AddUFunction(this, FName("OnPressedKeyboard"));
}

void UCStageClearItemWidget::AddEnemiesToList(const TArray<UCSpawnedEnemyData*>& InEnemyData)
{
	if (EnemyList)
	{
		EnemyList->ClearListItems();
		EnemiesToDisplay = InEnemyData;
		CurrentEnemyIndex = 0;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DisplayEnemy);
		if (EnemiesToDisplay.Num() > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_DisplayEnemy, this, &UCStageClearItemWidget::DisplayNextEnemy, 1.5f, true);
		}
	}
	if (TailBorder)
	{
		TailBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCStageClearItemWidget::OnPressedKeyboard(FKey Key)
{
	if (TailBorder && TailBorder->GetVisibility() == ESlateVisibility::Visible && OnGetItemDelegatePtr != nullptr)
	{
		for (auto& EnemyData : EnemiesToDisplay)
		{
			for (auto& DroppedItem : EnemyData->GetDroppedItems())
			{
				if (DroppedItem != nullptr) OnGetItemDelegatePtr->Broadcast(DroppedItem);
				else UE_LOG(LogTemp, Warning, TEXT("UCStageClearItemWidget : OnPressedKeyboard : Adding Unvalid ItemData To Inventory"));
			}
		}
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCStageClearItemWidget::DisplayNextEnemy()
{
	if (CurrentEnemyIndex < EnemiesToDisplay.Num())
	{
		if (EnemyList)
		{
			EnemyList->AddItem(EnemiesToDisplay[CurrentEnemyIndex]);
		}
		CurrentEnemyIndex++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DisplayEnemy);
		if (TailBorder)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_DisplayEnemy, FTimerDelegate::CreateLambda(
				[&]() {
					TailBorder->SetVisibility(ESlateVisibility::Visible);
				}), 1.f, false
			);
		}
	}
}