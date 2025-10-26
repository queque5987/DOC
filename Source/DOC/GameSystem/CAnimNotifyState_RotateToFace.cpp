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
	UE_LOG(LogStats, Log, TEXT("UCAnimNotifyState_RotateToFace"));
}

void UCAnimNotifyState_RotateToFace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (EnemyCharacter != nullptr)
	{
		ElapsedTime += FrameDeltaTime;
		EnemyCharacter->SetRotation(FMath::Lerp(StartRotation, TargetRotation, ElapsedTime / Duration));
		//TargetRotation = (EnemyCharacter->GetDealingCharacterLocation() - EnemyCharacter->GetLocation()).GetSafeNormal2D().Rotation();
		float CurrDist = FVector::Dist(EnemyCharacter->GetDealingCharacterLocation(), EnemyCharacter->GetLocation());
		UE_LOG(LogTemp, Log, TEXT("UCAnimNotifyState_RotateToFace CurrDist : %f"), CurrDist);
		if (bStepForward && CurrDist > Threshold)
		{
			FVector Direction = (EnemyCharacter->GetDealingCharacterLocation() - EnemyCharacter->GetLocation()).GetSafeNormal2D();
			EnemyCharacter->LaunchCharacter_Direction(Direction, StepForce_PerTick);
		}
	}
}
