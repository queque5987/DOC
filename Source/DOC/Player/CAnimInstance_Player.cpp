#include "Player/CAnimInstance_Player.h"
#include "Interfaces/IPlayerOnStage.h"

void UCAnimInstance_Player::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PlayerCharacter != nullptr)
	{
		MovementSpeed = PlayerCharacter->GetPlayerVelocity().Size();
		FVector2D RawVelocity = PlayerCharacter->GetMovementVector() * MovementSpeed;
		FVector2D Velocity = FMath::Lerp(PrevVelocity, RawVelocity, 0.2f);
		RightSpeed = Velocity.X;
		ForwardSpeed = Velocity.Y;

		PrevVelocity = Velocity;
	}
}

void UCAnimInstance_Player::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	PlayerCharacter = Cast<IIPlayerOnStage>(TryGetPawnOwner());
}
