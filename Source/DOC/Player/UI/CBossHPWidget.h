#pragma once

#include "CoreMinimal.h"
#include "Player/UI/CMonsterHPWidget.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CBossHPWidget.generated.h"

UCLASS()
class DOC_API UCBossHPWidget : public UCMonsterHPWidget
{
	GENERATED_BODY()
	
protected:
	FOnStatusChanged* StatusChanged_DelegatePtr;
	FDelegateHandle StatusChanged_DelegateHandle;
	FOnGroggyEnd* OnGroggyEnd_DelegatePtr;
	FDelegateHandle OnGroggyEnd_DelegateHandle;
	FTimerHandle HPDelayTimerHandle;

	bool GroggyBarOverride = false;
	float OverridenGroggyBarPercent = 1.f;
	float GroggyElipsedTime = 0.f;
	float GroggyTotalDuration = 5.f;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void SetupDelegate(FOnStatusChanged* InStatusChanged_DelegatePtr, FOnGroggyEnd* InGroggyEnd_DelegatePtr);
	virtual void UpdateBar(FPlayerStat MonsterStat) override;
	virtual void SetGroggyPanelVisibility(bool e) override;

	void SetGroggyDuration(float Sec) { GroggyTotalDuration = Sec; };
	UFUNCTION()
	void OnGroggyEnd();
};
