#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interfaces/IEnemyAIController.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "PCH.h"
#include "CAIController_Minion.generated.h"

UCLASS()
class DOC_API ACAIController_Minion : public AAIController, public IIEnemyAIController
{
	GENERATED_BODY()
	
	ACAIController_Minion(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	class UAISenseConfig_Sight* SightConfig;
	class UAIPerceptionComponent* AIPerceptionComponent;
	class UBlackboardComponent* BlackBoradComponent;
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	class IIEnemyCharacter* EnemyCharacter;
	class IINavSystemManager* NavSystemManager;
	class IIAnimInstance* IAnimInst;
	TQueue<int32> ActionBuffer;
	int32 LastAction = -1;
	int32 ComboAttackType;
	int32 ComboStack = 0;
	int32 MaxCombo;

	//bool bSeized;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	UFUNCTION()
	void SetPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus);

	virtual void OrderAction(int32 ActionType) override;
	virtual void SetupDelegates(FMONTAGE_PLAYING_STATE_CHANGED* Delegate_MontagePlayingStateChanged) override;
	virtual void CalculateRangedAttackPosition() override;
	virtual class AActor* GetCurrentAttackTargetActor() override;
	bool IsPlayerNear(float Distance);
};
