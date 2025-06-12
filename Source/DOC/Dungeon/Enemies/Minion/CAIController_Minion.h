#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController_Minion.generated.h"

UCLASS()
class DOC_API ACAIController_Minion : public AAIController
{
	GENERATED_BODY()
	
	ACAIController_Minion(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	class UAISenseConfig_Sight* SightConfig;
	class UAIPerceptionComponent* AIPerceptionComponent;
	class UBlackboardComponent* BlackBoradComponent;
	class UBehaviorTreeComponent* BehaviorTreeComponent;
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	UFUNCTION()
	void SetPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus);
};
