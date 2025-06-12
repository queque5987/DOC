#include "Dungeon/Enemies/Minion/CAnimInstance_Minion.h"
#include "Interfaces/IEnemyCharacter.h"

void UCAnimInstance_Minion::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if (TryGetPawnOwner() != nullptr)
	{
		EnemyCharacter = Cast<IIEnemyCharacter>(TryGetPawnOwner());
	}
}

void UCAnimInstance_Minion::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (EnemyCharacter != nullptr) MovementSpeed = EnemyCharacter->GetMovementSpeed();
}
