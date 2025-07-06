#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "CMonsterHP.generated.h"

UCLASS()
class DOC_API UCMonsterHP : public UWidgetComponent
{
	GENERATED_BODY()

private:
	// 가속도 관련 변수
	float CurrentInterpSpeed;
	const float DistanceThreshold = 100.0f; // 100cm 이상 벌어지면 가속 시작
	const float AngleThreshold = 30.0f;     // 30도 이상 벌어지면 가속 시작
	const float AccelerationRate = 2.0f;    // 초당 2.0씩 속도 증가
	const float BaseInterpSpeed = 1.5f;     // 기본 보간 속도
	const float MaxInterpSpeed = 15.0f;     // 최대 보간 속도
	float TimeAboveThreshold;               // 임계값 이상으로 벌어진 누적 시간

public:
	UCMonsterHP();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HPWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetMonsterHP(float CurrentHP, float MaxHP);
	void SetTransform(FVector NewLocation, FRotator NewRotation, float DeltaSeconds);
};
