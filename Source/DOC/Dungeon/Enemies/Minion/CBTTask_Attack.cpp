#include "Dungeon/Enemies/Minion/CBTTask_Attack.h"
#include "AIController.h"
#include "Interfaces/IEnemyAIController.h"

EBTNodeResult::Type UCBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Log, TEXT("UCBTTask_Attack : ExecuteTask : Attack"));
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;
	IIEnemyAIController* IAIController = Cast<IIEnemyAIController>(AIController);
	if (IAIController == nullptr) return EBTNodeResult::Failed;
	IAIController->OrderAction(0);
	return EBTNodeResult::Succeeded;
}
