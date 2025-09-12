#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Interfaces/CStageStructs.h"
#include "CAnimNotify_SpawnProjectile.generated.h"

UCLASS()
class DOC_API UCAnimNotify_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
public:
	UPROPERTY(EditAnywhere)
	FName FireSocketName;
	UPROPERTY(EditAnywhere)
	FDamageConfig DamageConfig;
};
