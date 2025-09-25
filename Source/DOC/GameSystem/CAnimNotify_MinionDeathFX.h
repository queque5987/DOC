#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_MinionDeathFX.generated.h"

UCLASS()
class DOC_API UCAnimNotify_MinionDeathFX : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
public:
	UPROPERTY(EditAnywhere)
	int32 FXSequence;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ToPlayParticleSystem = nullptr;
	UPROPERTY(EditAnywhere)
	FTransform PlayParticleAdjustTransform = FTransform();
};
