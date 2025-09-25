#include "GameSystem/CAnimNotify_MinionDeathFX.h"
#include "Interfaces/IEnemyCharacter.h"

void UCAnimNotify_MinionDeathFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	IIEnemyCharacter* EC = Cast<IIEnemyCharacter>(MeshComp->GetOwner());
	if (EC != nullptr) EC->PlayDiedFX(FXSequence, ToPlayParticleSystem, PlayParticleAdjustTransform);
}
