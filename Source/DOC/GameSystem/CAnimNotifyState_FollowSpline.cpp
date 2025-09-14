#include "GameSystem/CAnimNotifyState_FollowSpline.h"
#include "Interfaces/IEnemyCharacter.h"

void UCAnimNotifyState_FollowSpline::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		EnemyCharacter = Cast<IIEnemyCharacter>(MeshComp->GetOwner());
	}
	ElapsedTime = 0.0f;
	TotalTime = TotalDuration;
	LaunchForce = InitLaunchForce;
	if (bLaunchForward && EnemyCharacter != nullptr)
	{
		EnemyCharacter->LaunchCharacter_Direction(EnemyCharacter->GetForwardVector(), LaunchForce);
	}
}

void UCAnimNotifyState_FollowSpline::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	ElapsedTime += FrameDeltaTime;

	if (EnemyCharacter && MeshComp && MeshComp->GetOwner())
	{
		//if (bLaunchForward)
		//{
		//	EnemyCharacter->LaunchCharacter_Direction(EnemyCharacter->GetForwardVector(), LaunchForce);
		//	if (bAcc) LaunchForce += Acc * FrameDeltaTime;
		//}
		if (!bLaunchForward)
		{
			FTransform SplineTransform = EnemyCharacter->GetSplineTransformAtTime(ElapsedTime / TotalTime);
			FRotator Rotator = SplineTransform.GetRotation().Rotator();
			Rotator.Pitch = 0.f;
			Rotator.Roll = 0.f;
			MeshComp->GetOwner()->SetActorLocation(SplineTransform.GetLocation());
		}
	}
}

void UCAnimNotifyState_FollowSpline::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	EnemyCharacter = nullptr;
}
