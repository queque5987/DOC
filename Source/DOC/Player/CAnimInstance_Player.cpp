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

void UCAnimInstance_Player::PlayAnimation(UAnimSequenceBase* PlayAnimation, float BlendInTime, float BlendOutTime, float PlayRate, float StartTime)
{
	if (PlayAnimation != nullptr)
	{
		bBusy = true;
		Delegate_Montage_Playing_State_Changed.ExecuteIfBound(true);
		PlaySlotAnimationAsDynamicMontage(PlayAnimation, "DefaultSlot", BlendInTime, BlendOutTime, PlayRate, 1, -1.f, 0.f);
		CurrentPlayingAnimation = PlayAnimation;
	}
}

void UCAnimInstance_Player::SetBusy(bool e)
{
	bBusy = e;
	Delegate_Montage_Playing_State_Changed.ExecuteIfBound(e);
}

void UCAnimInstance_Player::SetGroggy(bool e)
{
	bGroggy = e;
}

void UCAnimInstance_Player::SetAirbone(bool e)
{
	bAirbone = e;
}

bool UCAnimInstance_Player::IsMontagePlaying()
{
	return IsAnyMontagePlaying();
}

void UCAnimInstance_Player::SetCounterReady(bool e)
{
	bCounterReady = e;
}

void UCAnimInstance_Player::SetCurrentMontagePlayRate(float PlayRate)
{
	if (GetCurrentActiveMontage() != nullptr) Montage_SetPlayRate(GetCurrentActiveMontage(), PlayRate);
}

void UCAnimInstance_Player::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		Delegate_MontagePlayerComboCleared.ExecuteIfBound();
	}
	//CurrentPlayingAnimation = nullptr;
}

void UCAnimInstance_Player::ReceiveDamage(FDamageConfig DamageConfig)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("UCAnimInstance_Player : ReceiveDamage : No Level Found"));
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(LastRecieveDamageTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(LastRecieveDamageTimerHandle,
		FTimerDelegate::CreateLambda([&] {
			SetBusy(false);
			}),
		0.55f, false);
}

UAnimSequenceBase* UCAnimInstance_Player::GetCurrentPlayingAnimation()
{
	return CurrentPlayingAnimation;
}

void UCAnimInstance_Player::StopAnimation()
{
	StopAllMontages(0.1f);
}

void UCAnimInstance_Player::SetupDelegates(FOnChangeCounterReady* OnChangeCounterReady, FOnReceivedDamage* InOnReceivedDamageDelegate, FOnGroggy* InOnGroggyDelegate, FOnGroggyEnd* InOnGroggyEndDeegate)
{
	if (OnChangeCounterReady != nullptr)
	{
		OnChangeCounterReady->AddUFunction(this, FName("OnChangeCounterReady_Callback"));
	}
	if (InOnReceivedDamageDelegate != nullptr)
	{
		InOnReceivedDamageDelegate->AddUFunction(this, FName("ReceiveDamage"));
	}
}

void UCAnimInstance_Player::OnChangeCounterReady_Callback(bool bReady)
{
	bCounterReady = bReady;
}