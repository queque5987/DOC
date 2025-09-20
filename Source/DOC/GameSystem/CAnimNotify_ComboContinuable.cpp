#include "GameSystem/CAnimNotify_ComboContinuable.h"
#include "Interfaces/IAnimInstance.h"

void UCAnimNotify_ComboContinuable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	IIAnimInstance* IAnimInst = Cast<IIAnimInstance>(MeshComp->GetAnimInstance());
	if (IAnimInst != nullptr)
	{
		UAnimSequenceBase* CurrentPlayingAnim = IAnimInst->GetCurrentPlayingAnimation();
		if (CurrentPlayingAnim != nullptr && Animation != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("UCAnimNotify_ComboContinuable: Current Playing Anim Name : %s"), *CurrentPlayingAnim->GetName());
			UE_LOG(LogTemp, Log, TEXT("UCAnimNotify_ComboContinuable: Param Anim Name : %s"), *Animation->GetName());
		}
		if (CurrentPlayingAnim == Animation) IAnimInst->SetBusy(false);
	}
}
