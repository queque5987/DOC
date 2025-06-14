#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "IEnemyAIController.generated.h"

UINTERFACE(MinimalAPI)
class UIEnemyAIController : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIEnemyAIController
{
	GENERATED_BODY()

public:
	virtual void OrderAction(int32 ActionType) {};
	virtual void SetupDelegates(FMONTAGE_PLAYING_STATE_CHANGED* Delegate_EnemyMontagePlayingStateChanged) {};
};
