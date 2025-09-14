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
	float ElipsedTime;
	float HitTickCount;
public:
	FCollisionShape SweepShape;
	UPROPERTY(EditAnywhere)
	float Damage = 1.f;
	UPROPERTY(EditAnywhere, Category = "Socket")
	FName SocketName_0;
	UPROPERTY(EditAnywhere, Category = "Socket")
	FName SocketName_1;
	UPROPERTY(EditAnywhere, Category = "Damage")
	bool bHitMultipleTime = false;
	UPROPERTY(EditAnywhere, Category = "Damage")
	float TickInterval = 0.25f;
	UPROPERTY(EditAnywhere, Category = "Damage")
	FDamageConfig DamageConfig;
	UPROPERTY(EditAnywhere, Category = "Socket")
	float SweepShapeThickness = 10.f;
	UPROPERTY(EditAnywhere, Category = "Socket")
	float SweepShapeLength = 1.f;
};
