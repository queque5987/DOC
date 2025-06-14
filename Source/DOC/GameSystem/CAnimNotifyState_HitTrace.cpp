#include "GameSystem/CAnimNotifyState_HitTrace.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interfaces/IEnemyCharacter.h"
#include "DrawDebugHelpers.h"

UCAnimNotifyState_HitTrace::UCAnimNotifyState_HitTrace()
{
	
}

void UCAnimNotifyState_HitTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	SweepShape.SetCapsule(
		10.f,
		FVector::Dist(
			MeshComp->GetSocketLocation(SocketName_0),
			MeshComp->GetSocketLocation(SocketName_1)
		) * 0.5f
	);
	EnemyCharacter = Cast<IIEnemyCharacter>(MeshComp->GetOwner());
	if (EnemyCharacter!= nullptr) EnemyCharacter->ResetTraceProperties();
	UE_LOG(LogTemp, Log, TEXT("UCAnimNotifyState_HitTrace : NotifyBegin"));
}

void UCAnimNotifyState_HitTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (EnemyCharacter != nullptr)
	{
		EnemyCharacter->PerformCapsuleTrace(
			SweepShape.GetCapsuleRadius(),
			SweepShape.GetCapsuleHalfHeight(),
			(MeshComp->GetSocketLocation(SocketName_0) + MeshComp->GetSocketLocation(SocketName_1)) / 2.f,
			(MeshComp->GetSocketLocation(SocketName_1) - MeshComp->GetSocketLocation(SocketName_0)).GetSafeNormal().Rotation() + FRotator(90.f, 0.f, 0.f),
			5,
			Damage
		);
	}
	//DrawDebugCapsule(MeshComp->GetWorld(),
	//	(MeshComp->GetSocketLocation(SocketName_0) + MeshComp->GetSocketLocation(SocketName_1)) / 2.f,
	//	SweepShape.GetCapsuleHalfHeight(),
	//	SweepShape.GetCapsuleRadius(),
	//	((MeshComp->GetSocketLocation(SocketName_1) - MeshComp->GetSocketLocation(SocketName_0)).GetSafeNormal().Rotation() + FRotator(90.f, 0.f, 0.f)).Quaternion(),
	//	FColor::White,
	//	false,
	//	1.f,
	//	0U,
	//	1.f
	//);
}
