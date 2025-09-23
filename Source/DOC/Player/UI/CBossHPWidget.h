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
public:
	void SetupDelegate(FOnStatusChanged* InStatusChanged_DelegatePtr, FOnGroggyEnd* InGroggyEnd_DelegatePtr);
	virtual void UpdateBar(FPlayerStat MonsterStat) override;

	UFUNCTION()
	void OnGroggyEnd();
};
