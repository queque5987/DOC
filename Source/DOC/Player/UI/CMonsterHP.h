#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Interfaces/CStageStructs.h"
#include "Blueprint/UserWidget.h"
#include "CMonsterHP.generated.h"

UCLASS()
class DOC_API UCMonsterHP : public UWidgetComponent
{
	GENERATED_BODY()

private:
	float CurrentInterpSpeed;
	const float DistanceThreshold = 50.0f; 
	const float AngleThreshold = 15.0f;    
	const float AccelerationRate = 2.0f;   
	const float BaseInterpSpeed = 3.f;     
	const float MaxInterpSpeed = 30.0f;     
	float TimeAboveThreshold;               

public:
	UCMonsterHP();

	void SetTransform(FVector NewLocation, FRotator NewRotation, float DeltaSeconds);
	void SetDelegates(FOnStatusChanged* InStatusChangedDelegate);

private:
	//FHP_CHANGED* HPChangedDelegate;
	FOnStatusChanged* StatusChangedDelegate;

	UPROPERTY()
	class UCMonsterHPWidget* MonsterHPWidgetInstance;

	UFUNCTION()
	void UpdateHP(FPlayerStat MonsterStat);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> HPWidgetClass;

	FTimerHandle HPDelayTimerHandle;
	UFUNCTION()
	void OnHPDelayTimerExpired();
};
