#include "GameSystem/CAnimNotify_ComboContinuable.h"
#include "Interfaces/IAnimInstance.h"

void UCAnimNotify_ComboContinuable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	IIAnimInstance* IAnimInst = Cast<IIAnimInstance>(MeshComp->GetAnimInstance());
	if (IAnimInst != nullptr)
	{
		IAnimInst->SetBusy(false);
	}
}
