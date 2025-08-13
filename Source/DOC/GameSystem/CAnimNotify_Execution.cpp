#include "GameSystem/CAnimNotify_Execution.h"
#include "Interfaces/IDamagable.h"

void UCAnimNotify_Execution::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp->GetOwner())
	{
		IIDamagable* Damagable = Cast<IIDamagable>(MeshComp->GetOwner());
		if (Damagable)
		{
			Damagable->Execute(DamageConfig);
		}
	}
}