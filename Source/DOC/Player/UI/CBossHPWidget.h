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
public:
	void SetupDelegate(FOnStatusChanged* InStatusChanged_DelegatePtr);
};
