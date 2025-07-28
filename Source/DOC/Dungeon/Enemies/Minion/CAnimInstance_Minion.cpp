#include "Dungeon/Enemies/Minion/CAnimInstance_Minion.h"
#include "Interfaces/IEnemyCharacter.h"

void UCAnimInstance_Minion::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OnMontageEnded.AddDynamic(this, &UCAnimInstance_Minion::OnMontageEnd);
}

void UCAnimInstance_Minion::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (EnemyCharacter != nullptr) MovementSpeed = EnemyCharacter->GetMovementSpeed();
}

void UCAnimInstance_Minion::OnPossess(IIEnemyCharacter* PossessCharacter)
{
	EnemyCharacter = PossessCharacter;
}

void UCAnimInstance_Minion::PlayAnimation(UAnimSequenceBase* PlayAnimation, float BlendInTime, float BlendOutTime, float PlayRate)
{
	bBusy = true;
	Delegate_Montage_Playing_State_Changed.ExecuteIfBound(true);
	PlaySlotAnimationAsDynamicMontage(PlayAnimation, "DefaultSlot", BlendInTime, BlendOutTime, PlayRate);
}

void UCAnimInstance_Minion::SetBusy(bool e)
{
	Delegate_Montage_Playing_State_Changed.ExecuteIfBound(e);
	bBusy = e;
}

bool UCAnimInstance_Minion::GetBusy()
{
	return bBusy;
}

void UCAnimInstance_Minion::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	Delegate_Montage_Playing_State_Changed.ExecuteIfBound(false);
}
