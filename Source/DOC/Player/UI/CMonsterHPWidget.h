#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "CMonsterHPWidget.generated.h"

UCLASS()
class DOC_API UCMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar_Delay;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHPBar(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetDelayHP(float NewDelayHPPercent);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
private:
	float TargetHealthPercent; // HPBar가 즉시 반영할 목표 HP 비율
	float CurrentDelayHealthPercent; // HPBar_Delay가 현재 표시하는 HP 비율
	const float DelayInterpSpeed = 2.0f; // HPBar_Delay가 따라가는 속도 (조절 가능)

	bool bIsDelaying; // 지연 보간이 활성화되었는지 여부

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartDelayInterpolation();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void StopDelayInterpolation();
};