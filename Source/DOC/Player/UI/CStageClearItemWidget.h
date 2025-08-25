#pragma once

#include "CoreMinimal.h"
#include "Player/UI/CChestItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CStageClearItemWidget.generated.h"

class UListView;
class UBorder;

UCLASS()
class DOC_API UCStageClearItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void SetupDelegates(FPressedKeyboard* InOnPressedKeyboardDelegatePtr, FGETITEM* InOnGetItemDelegatePtr);
	void AddEnemiesToList(const TArray<UCSpawnedEnemyData*>& InEnemyData);
	UFUNCTION()
	void OnPressedKeyboard(FKey Key);
protected:
	UPROPERTY(meta = (BindWidget))
	UListView* EnemyList;

	UPROPERTY(meta = (BindWidget))
	UBorder* TailBorder;
	
	FGETITEM* OnGetItemDelegatePtr;
	FPressedKeyboard* OnPressedKeyboardDelegatePtr;
private:
	TArray<UCSpawnedEnemyData*> EnemiesToDisplay;
	FTimerHandle TimerHandle_DisplayEnemy;
	int32 CurrentEnemyIndex;
	void DisplayNextEnemy();
};
