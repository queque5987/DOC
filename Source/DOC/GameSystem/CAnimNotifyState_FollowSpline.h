#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CAnimNotifyState_FollowSpline.generated.h"

UCLASS()
class DOC_API UCAnimNotifyState_FollowSpline : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Launch")
	bool bLaunchForward = false;
	UPROPERTY(EditAnywhere, Category = "Launch")
	float InitLaunchForce = 0.f;
	UPROPERTY(EditAnywhere, Category = "Launch")
	bool bAcc = false;
	UPROPERTY(EditAnywhere, Category = "Launch")
	float Acc = 0.f;
private:
	class IIEnemyCharacter* EnemyCharacter = nullptr;
	float TotalTime = 0.f;
	float ElapsedTime = 0.f;
	float LaunchForce = 0.f;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
