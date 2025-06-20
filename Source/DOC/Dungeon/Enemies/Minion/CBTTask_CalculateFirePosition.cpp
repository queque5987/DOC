#include "Dungeon/Enemies/Minion/CBTTask_CalculateFirePosition.h"
#include "AIController.h"
#include "Interfaces/IEnemyAIController.h"

EBTNodeResult::Type UCBTTask_CalculateFirePosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Log, TEXT("UCBTTask_Attack : ExecuteTask : CalculateFirePosition"));
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;
	IIEnemyAIController* IAIController = Cast<IIEnemyAIController>(AIController);
	if (IAIController == nullptr) return EBTNodeResult::Failed;
	IAIController->CalculateRangedAttackPosition();
	return EBTNodeResult::Succeeded;
}
