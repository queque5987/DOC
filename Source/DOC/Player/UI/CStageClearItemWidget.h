#pragma once

#include "CoreMinimal.h"
#include "Player/UI/CChestItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CStageClearItemWidget.generated.h"

class UListView;
class UBorder;

UCLASS()
class DOC_API UCStageClearItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void AddEnemiesToList(const TArray<UCSpawnedEnemyData*>& InEnemyData);

protected:
	UPROPERTY(meta = (BindWidget))
	UListView* EnemyList;

	UPROPERTY(meta = (BindWidget))
	UBorder* AroundBorder;
	
private:
	TArray<UCSpawnedEnemyData*> EnemiesToDisplay;
	FTimerHandle TimerHandle_DisplayEnemy;
	int32 CurrentEnemyIndex;
	void DisplayNextEnemy();
};
