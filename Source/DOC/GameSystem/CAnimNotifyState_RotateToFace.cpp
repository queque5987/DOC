#include "GameSystem/CAnimNotifyState_RotateToFace.h"
#include "Interfaces/IEnemyCharacter.h"

void UCAnimNotifyState_RotateToFace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	EnemyCharacter = Cast<IIEnemyCharacter>(MeshComp->GetOwner());
	if (EnemyCharacter != nullptr)
	{
		TargetRotation = (EnemyCharacter->GetDealingCharacterLocation() - EnemyCharacter->GetLocation()).GetSafeNormal2D().Rotation();
		StartRotation = EnemyCharacter->GetRotation();
	}
	Duration = TotalDuration;
	ElapsedTime = 0.f;
}

void UCAnimNotifyState_RotateToFace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (EnemyCharacter != nullptr)
	{
		ElapsedTime += FrameDeltaTime;
		EnemyCharacter->SetRotation(FMath::Lerp(StartRotation, TargetRotation, ElapsedTime / Duration));
	}
}
