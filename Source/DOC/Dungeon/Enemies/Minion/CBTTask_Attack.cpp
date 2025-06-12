#include "Dungeon/Enemies/Minion/CBTTask_Attack.h"

EBTNodeResult::Type UCBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Log, TEXT("UCBTTask_Attack : ExecuteTask : Attack"));

	return EBTNodeResult::Succeeded;
}
