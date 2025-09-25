#include "Dungeon/Enemies/Boss/CAnimInstance_Boss.h"
#include "CAnimInstance_Boss.h"

void UCAnimInstance_Boss::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (TryGetPawnOwner()) MovementSpeed = TryGetPawnOwner()->GetVelocity().Size();
	MovementDirection = FVector();
	FVector ForwardDirection = GetOwningActor()->GetActorForwardVector();
	float D = FVector::DotProduct(MovementDirection, ForwardDirection);
	D = FMath::Clamp(D, -1.f, 1.f);
	fMovementDirection = FMath::Acos(D);
	if (FVector::CrossProduct(MovementDirection, ForwardDirection).Z < 0)
	{
		fMovementDirection = -fMovementDirection;
	}
	//UE_LOG(LogTemp, Log, TEXT("UCAnimInstance_Boss : NativeUpdateAnimation : MovementDirection : %s"), *MovementDirection.ToString());
}

FVector UCAnimInstance_Boss::GetMovementDirectionVector()
{
	return TryGetPawnOwner() != nullptr ? TryGetPawnOwner()->GetVelocity() : FVector();
}

void UCAnimInstance_Boss::OnPossess(IIEnemyCharacter* PossessCharacter)
{
	Super::OnPossess(PossessCharacter);
}
