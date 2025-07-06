#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Blueprint/UserWidget.h"
#include "CMonsterHP.generated.h"

UCLASS()
class DOC_API UCMonsterHP : public UWidgetComponent
{
	GENERATED_BODY()

private:
	float CurrentInterpSpeed;
	const float DistanceThreshold = 50.0f; // 가속 시작
	const float AngleThreshold = 15.0f;     // 가속 시작
	const float AccelerationRate = 2.0f;    // 초당 2.0씩 속도 증가
	const float BaseInterpSpeed = 3.f;     // 기본 보간 속도
	const float MaxInterpSpeed = 30.0f;     // 최대 보간 속도
	float TimeAboveThreshold;               // 임계값 이상으로 벌어진 누적 시간

public:
	UCMonsterHP();

	void SetTransform(FVector NewLocation, FRotator NewRotation, float DeltaSeconds);
	void SetDelegates(FHP_CHANGED* InHPChangedDelegate, FHP_DELAY_UPDATE_INIT* InHPDelayUpdateInitDelegate);

private:
	FHP_CHANGED* HPChangedDelegate;
	FHP_DELAY_UPDATE_INIT* HPDelayUpdateInitDelegate;

	UPROPERTY()
	class UCMonsterHPWidget* MonsterHPWidgetInstance;

	UFUNCTION()
	void UpdateHP(float NewHP, float MaxHP);

	UFUNCTION()
	void OnHPDelayUpdateInitCallback();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> HPWidgetClass;
};
