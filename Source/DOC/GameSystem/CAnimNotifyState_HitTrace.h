#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Interfaces/CStageStructs.h"
#include "CAnimNotifyState_HitTrace.generated.h"

UCLASS()
class DOC_API UCAnimNotifyState_HitTrace : public UAnimNotifyState
{
	GENERATED_BODY()

	UCAnimNotifyState_HitTrace();
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	//virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
protected:
	FVector Prev_SocketLocation;
	FRotator Prev_SocketRotation;
	class IIEnemyCharacter* EnemyCharacter;
	class IIDamagable* Damagable;
public:
	FCollisionShape SweepShape;
	UPROPERTY(EditAnywhere)
	float Damage = 1.f;
	UPROPERTY(EditAnywhere)
	FName SocketName_0;
	UPROPERTY(EditAnywhere)
	FName SocketName_1;
	UPROPERTY(EditAnywhere)
	FDamageConfig DamageConfig;
};
