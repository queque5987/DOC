#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CAnimNotifyState_RotateToFace.generated.h"

UCLASS()
class DOC_API UCAnimNotifyState_RotateToFace : public UAnimNotifyState
{
	GENERATED_BODY()
	class IIEnemyCharacter* EnemyCharacter;
	FRotator TargetRotation;
	FRotator StartRotation;
	float ElapsedTime;
	float Duration;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
};
