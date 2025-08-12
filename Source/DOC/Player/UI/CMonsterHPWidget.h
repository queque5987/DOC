#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/PanelWidget.h"
#include "Interfaces/CStageStructs.h"
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

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* GroggyBar;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* CounterPannel;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHPBar(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateBar(FPlayerStat MonsterStat);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetDelayHP(float NewDelayHPPercent);
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
private:
	float TargetHealthPercent;
	float TargetGroggyPercent;
	float CurrentDelayHealthPercent;
	const float DelayInterpSpeed = 2.0f;

	bool bIsDelaying;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartDelayInterpolation();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void StopDelayInterpolation();

	void SetGroggyPanelVisibility(bool e);
};