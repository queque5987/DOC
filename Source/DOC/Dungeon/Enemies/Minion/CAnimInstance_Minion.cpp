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

void UCAnimInstance_Minion::SetupDelegates(FOnChangeCounterReady* OnChangeCounterReady, FOnReceivedDamage* InOnReceivedDamageDelegate, FOnGroggy* InOnGroggyDelegate, FOnGroggyEnd* InOnGroggyEndDeegate)
{
	if (OnReceivedDamageDele != nullptr && OnReceivedDamage_Callback_Handle.IsValid())
	{
		OnReceivedDamageDele->Remove(OnReceivedDamage_Callback_Handle);
	}
	OnReceivedDamageDele = InOnReceivedDamageDelegate;
	if (OnReceivedDamageDele != nullptr)
	{
		OnReceivedDamage_Callback_Handle = OnReceivedDamageDele->AddUFunction(this, TEXT("OnReceivedDamage"));
	}

	if (OnGroggyDelegate != nullptr && OnGroggy_Callback_Handle.IsValid())
	{
		OnGroggyDelegate->Remove(OnGroggy_Callback_Handle);
	}
	OnGroggyDelegate = InOnGroggyDelegate;
	if (OnGroggyDelegate != nullptr)
	{
		OnGroggy_Callback_Handle = OnGroggyDelegate->AddUFunction(this, TEXT("OnGroggy"));
	}

	if (OnGroggyEndDelegate != nullptr && OnGroggyEnd_Callback_Handle.IsValid())
	{
		OnGroggyEndDelegate->Remove(OnGroggyEnd_Callback_Handle);
	}
	OnGroggyEndDelegate = InOnGroggyEndDeegate;
	if (OnGroggyEndDelegate != nullptr)
	{
		OnGroggyEnd_Callback_Handle = OnGroggyEndDelegate->AddUFunction(this, TEXT("OnGroggyEnd"));
	}
}

void UCAnimInstance_Minion::OnPossess(IIEnemyCharacter* PossessCharacter)
{
	EnemyCharacter = PossessCharacter;
	Deceased = false;
}

void UCAnimInstance_Minion::PlayAnimation(UAnimSequenceBase* PlayAnimation, float BlendInTime, float BlendOutTime, float PlayRate, float StartTime)
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

void UCAnimInstance_Minion::OnGroggy(FDamageConfig DamageConfig)
{
	bGroggy = true;
}

void UCAnimInstance_Minion::OnReceivedDamage(FDamageConfig DamageConfig)
{
	//StopAllMontages(0.15f);

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
			LocalHitDirectionType = 0; //Forward
		}
		else if (DotForward >= -Cos45Deg)
		{
			LocalHitDirectionType = 1; //Backward
		}
		else if (DotRight < 0.0f)
		{
			LocalHitDirectionType = 3; //Right
		}
		else
		{
			LocalHitDirectionType = 2; //Left
		}
		PlayAnimation(EnemyCharacter->GetHitReactAnimSequence(LocalHitDirectionType));
		if (!GetWorld())
		{
			UE_LOG(LogTemp, Error, TEXT("UCAnimInstance_Minion : OnReceivedDamage : No Level Found"));
		}
		GetWorld()->GetTimerManager().ClearTimer(LastRecieveDamageTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(LastRecieveDamageTimerHandle,
			FTimerDelegate::CreateLambda([&] {
				SetBusy(false);
			}),
			1.f, false);
		//DrawDebugDirectionalArrow(GetWorld(), CharacterLocation - CharacterForward * 300.0f, CharacterLocation, 20.0f, FColor::Green, false, 2.0f, 0, 2.0f);
		//DrawDebugDirectionalArrow(GetWorld(), CharacterLocation - NormalizedHitDirection * 300.0f, CharacterLocation, 20.0f, FColor::Red, false, 2.0f, 0, 2.0f);
	}
}

void UCAnimInstance_Minion::OnGroggyEnd()
{
	bGroggy = false;
}
