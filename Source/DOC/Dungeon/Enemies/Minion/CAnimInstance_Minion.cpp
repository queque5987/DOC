#include "Dungeon/Enemies/Minion/CAnimInstance_Minion.h"
#include "Interfaces/IEnemyCharacter.h"
#include "DrawDebugHelpers.h"

void UCAnimInstance_Minion::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	bEnableRigidBody = false;
	OnMontageEnded.AddDynamic(this, &UCAnimInstance_Minion::OnMontageEnd);
}

void UCAnimInstance_Minion::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (EnemyCharacter != nullptr) MovementSpeed = EnemyCharacter->GetMovementSpeed();
}

void UCAnimInstance_Minion::SetupDelegates(FOnChangeCounterReady* OnChangeCounterReady, FOnReceivedDamage* InOnReceivedDamageDelegate)
{
	OnReceivedDamageDele = InOnReceivedDamageDelegate;
	if (OnReceivedDamageDele != nullptr)
	{
		if (OnReceivedDamage_Callback_Handle.IsValid()) OnReceivedDamageDele->Remove(OnReceivedDamage_Callback_Handle);
		OnReceivedDamage_Callback_Handle = OnReceivedDamageDele->AddUFunction(this, TEXT("OnReceivedDamage"));
	}
}

void UCAnimInstance_Minion::OnPossess(IIEnemyCharacter* PossessCharacter)
{
	EnemyCharacter = PossessCharacter;
	Deceased = false;
}

void UCAnimInstance_Minion::PlayAnimation(UAnimSequenceBase* PlayAnimation, float BlendInTime, float BlendOutTime, float PlayRate)
{
	if (PlayAnimation != nullptr)
	{
		bBusy = true;
		Delegate_Montage_Playing_State_Changed.ExecuteIfBound(true);
		PlaySlotAnimationAsDynamicMontage(PlayAnimation, "DefaultSlot", BlendInTime, BlendOutTime, PlayRate);
	}
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

void UCAnimInstance_Minion::Died(FDamageConfig DamageConfig)
{
	Deceased = true;
}

void UCAnimInstance_Minion::OnReceivedDamage(FDamageConfig DamageConfig)
{
	StopAllMontages(0.15f);

	if (EnemyCharacter)
	{
		FVector CharacterLocation = EnemyCharacter->GetLocation();
		FVector CharacterForward = EnemyCharacter->GetForwardVector();
		FVector CharacterRight = FVector::CrossProduct(FVector::UpVector, CharacterForward);

		FVector NormalizedHitDirection = DamageConfig.HitDirection.GetSafeNormal();

		float DotForward = FVector::DotProduct(CharacterForward, NormalizedHitDirection);
		float DotRight = FVector::DotProduct(CharacterRight, NormalizedHitDirection);

		int32 LocalHitDirectionType = 0;

		float Cos45Deg = FMath::Cos(FMath::DegreesToRadians(45.0f));

		if (DotForward < Cos45Deg)
		{
			LocalHitDirectionType = 0;
			UE_LOG(LogTemp, Log, TEXT("Hit Direction Type: Forward"));
		}
		else if (DotForward >= -Cos45Deg)
		{
			LocalHitDirectionType = 1;
			UE_LOG(LogTemp, Log, TEXT("Hit Direction Type: Backward"));
		}
		else if (DotRight < 0.0f)
		{
			LocalHitDirectionType = 3;
			UE_LOG(LogTemp, Log, TEXT("Hit Direction Type: Right"));
		}
		else
		{
			LocalHitDirectionType = 2;
			UE_LOG(LogTemp, Log, TEXT("Hit Direction Type: Left"));
		}
		PlayAnimation(EnemyCharacter->GetHitReactAnimSequence(LocalHitDirectionType));
		// Draw Debug Arrows
		DrawDebugDirectionalArrow(GetWorld(), CharacterLocation - CharacterForward * 300.0f, CharacterLocation, 20.0f, FColor::Green, false, 2.0f, 0, 2.0f);
		DrawDebugDirectionalArrow(GetWorld(), CharacterLocation - NormalizedHitDirection * 300.0f, CharacterLocation, 20.0f, FColor::Red, false, 2.0f, 0, 2.0f);
	}
}
