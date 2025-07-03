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
		float rootX = GetCurveValue("root_translation_X");
		float rootY = GetCurveValue("root_translation_Y");
		bool bLaunch = rootY <= 0.f ? false : true;
		FVector RootPos{ rootY, rootX, 0.f };
		PlayerCharacter->AdjustRootBone(PrevRootPos - RootPos, bLaunch, false);
		PrevRootPos = RootPos;
	}
}

void UCAnimInstance_Player::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	PlayerCharacter = Cast<IIPlayerOnStage>(TryGetPawnOwner());
	OnMontageEnded.AddDynamic(this, &UCAnimInstance_Player::OnMontageEnd);
}

void UCAnimInstance_Player::PlayAnimation(UAnimSequenceBase* PlayAnimation)
{
	bBusy = true;
	Delegate_Montage_Playing_State_Changed.ExecuteIfBound(true);
	PlaySlotAnimationAsDynamicMontage(PlayAnimation, "DefaultSlot");
}

void UCAnimInstance_Player::SetBusy(bool e)
{
	bBusy = e;
	Delegate_Montage_Playing_State_Changed.ExecuteIfBound(e);
}

bool UCAnimInstance_Player::IsMontagePlaying()
{
	return IsAnyMontagePlaying();
}

void UCAnimInstance_Player::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted) Delegate_MontagePlayerComboCleared.ExecuteIfBound();
}
