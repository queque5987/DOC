#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interfaces/IEnemyAIController.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Interfaces/CStageStructs.h"
#include "PCH.h"
#include "CAIController_Boss.generated.h"

UCLASS()
class DOC_API ACAIController_Boss : public AAIController, public IIEnemyAIController
{
	GENERATED_BODY()

	ACAIController_Boss(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	class UAISenseConfig_Sight* SightConfig;
	class UAIPerceptionComponent* AIPerceptionComponent;
	class UBlackboardComponent* BlackBoradComponent;
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	class IIEnemyCharacter* EnemyCharacter;

	class AActor* DetectedPlayer;
	TQueue<int32> ActionBuffer;

	FOnEnemyAction* OnEnemyActionDelegatePtr;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void OrderAction(int32 ActionType) override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void SetPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus);
};
