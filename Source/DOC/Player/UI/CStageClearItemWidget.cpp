#include "Player/UI/CStageClearItemWidget.h"
#include "Components/ListView.h"
#include "Components/Border.h"
#include "Player/UI/CSpawnedEnemyData.h"
#include "TimerManager.h" // Include for FTimerHandle and GetWorld()->GetTimerManager()

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
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_DisplayEnemy, this, &UCStageClearItemWidget::DisplayNextEnemy, 0.5f, true);
		}
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
	}
}