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
		
		//GEngine->AddOnScreenDebugMessage(1, DeltaSeconds, FColor::Green, FString::Printf(TEXT("Forward Speed : %f"), ForwardSpeed));

		FRotator AdjustRotation{
			GetCurveValue("root_rotation_Y"),
			GetCurveValue("root_rotation_Z"),
			GetCurveValue("root_rotation_X")
		};
		float rootZ = GetCurveValue("root_translation_Z");
		if (rootZ == 0.f) rootZ = GetCurveValue("Roll_translation_Z");;
		RootZStack += PrevRootZ - rootZ;
		float rootZstep = RootZStack * FMath::Min(1.f, DeltaSeconds * 20.f);
		FVector AdjustVertial{
			0.f, 
			0.f, 
			rootZstep
		};
		RootZStack -= rootZstep;
		float rootX = GetCurveValue("root_translation_X");
		float rootY = GetCurveValue("root_translation_Y");
		FVector RootPos{ rootY, rootX, 0.f };

		PlayerCharacter->AdjustMesh(AdjustVertial, PrevRootRot - AdjustRotation, PrevRootPos - RootPos);

		PrevRootRot = AdjustRotation;
		PrevRootZ = rootZ;
		PrevRootPos = RootPos;
	}
}

void UCAnimInstance_Player::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	PlayerCharacter = Cast<IIPlayerOnStage>(TryGetPawnOwner());
	OnMontageEnded.AddDynamic(this, &UCAnimInstance_Player::OnMontageEnd);
}

void UCAnimInstance_Player::PlayAnimation(UAnimSequenceBase* PlayAnimation, float BlendInTime, float BlendOutTime, float PlayRate)
{
	bBusy = true;
	Delegate_Montage_Playing_State_Changed.ExecuteIfBound(true);
	PlaySlotAnimationAsDynamicMontage(PlayAnimation, "DefaultSlot", BlendInTime, BlendOutTime, PlayRate);
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

void UCAnimInstance_Player::SetCounterReady(bool e)
{
	bCounterReady = e;
}

void UCAnimInstance_Player::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted) Delegate_MontagePlayerComboCleared.ExecuteIfBound();
}

void UCAnimInstance_Player::SetupDelegates(FOnChangeCounterReady* OnChangeCounterReady, FOnReceivedDamage* InOnReceivedDamageDelegate)
{
	if (OnChangeCounterReady != nullptr)
	{
		OnChangeCounterReady->AddUFunction(this, FName("OnChangeCounterReady_Callback"));
	}
}

void UCAnimInstance_Player::OnChangeCounterReady_Callback(bool bReady)
{
	bCounterReady = bReady;
}