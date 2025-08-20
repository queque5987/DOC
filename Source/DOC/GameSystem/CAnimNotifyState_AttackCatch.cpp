#include "GameSystem/CAnimNotifyState_AttackCatch.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Interfaces/IDamagable.h"

void UCAnimNotifyState_AttackCatch::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	bCatched = false;
	SweepShape.SetCapsule(
		10.f,
		FVector::Dist(
			MeshComp->GetSocketLocation(SocketName_0),
			MeshComp->GetSocketLocation(SocketName_1)
		) * 0.5f
	);
	Damagable = Cast<IIDamagable>(MeshComp->GetOwner());
	if (Damagable != nullptr)
	{
		Damagable->ResetTraceProperties();
		Damagable->SetInvincibleMoment(0.5f, false);
	}
	InvincibleDuration = TotalDuration + 0.5f;
}

void UCAnimNotifyState_AttackCatch::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	InvincibleDuration -= FrameDeltaTime;
	bool tempCatched = false;
	if (Damagable != nullptr)
	{
		tempCatched = Damagable->PerformCapsuleTrace(
			SweepShape.GetCapsuleRadius(),
			SweepShape.GetCapsuleHalfHeight(),
			(MeshComp->GetSocketLocation(SocketName_0) + MeshComp->GetSocketLocation(SocketName_1)) / 2.f,
			(MeshComp->GetSocketLocation(SocketName_1) - MeshComp->GetSocketLocation(SocketName_0)).GetSafeNormal().Rotation() + FRotator(90.f, 0.f, 0.f),
			5,
			DamageConfig
		);
	}
	if (!bCatched && tempCatched)
	{
		bCatched = tempCatched;
		Damagable->SetInvincibleMoment(InvincibleDuration, false);
	}
}

void UCAnimNotifyState_AttackCatch::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (Damagable != nullptr)
	{
		//Pause For a Sec
		Damagable->Catch(0.25f, 0.1f, CatchedDamageConfig);
		//TODO Manual Groggy
	}
}
