#include "GameSystem/CAnimNotifyState_HitTrace.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Interfaces/IDamagable.h"
#include "DrawDebugHelpers.h"

UCAnimNotifyState_HitTrace::UCAnimNotifyState_HitTrace()
{
	
}

void UCAnimNotifyState_HitTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	SweepShape.SetCapsule(
		SweepShapeThickness,
		FVector::Dist(
			MeshComp->GetSocketLocation(SocketName_0),
			MeshComp->GetSocketLocation(SocketName_1)
		) * 0.5f * SweepShapeLength
	);
	Damagable = Cast<IIDamagable>(MeshComp->GetOwner());
	if (Damagable != nullptr) Damagable->ResetTraceProperties();
	HitTickCount = TickInterval;
	ElipsedTime = 0.f;
}

void UCAnimNotifyState_HitTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	ElipsedTime += FrameDeltaTime;
	if (Damagable != nullptr)
	{
		if (bHitMultipleTime && ElipsedTime >= HitTickCount)
		{
			HitTickCount += TickInterval;
			Damagable->ResetTraceProperties();
		}

		Damagable->PerformCapsuleTrace(
			SweepShape.GetCapsuleRadius(),
			SweepShape.GetCapsuleHalfHeight(),
			(MeshComp->GetSocketLocation(SocketName_0) + MeshComp->GetSocketLocation(SocketName_1)) / 2.f,
			(MeshComp->GetSocketLocation(SocketName_1) - MeshComp->GetSocketLocation(SocketName_0)).GetSafeNormal().Rotation() + FRotator(90.f, 0.f, 0.f),
			5,
			DamageConfig
		);
	}
}
