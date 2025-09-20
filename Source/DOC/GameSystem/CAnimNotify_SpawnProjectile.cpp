#include "GameSystem/CAnimNotify_SpawnProjectile.h"
#include "Interfaces/IDamagable.h"
#include "Engine/SkeletalMeshSocket.h"

void UCAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		IIDamagable* Damagable = Cast<IIDamagable>(MeshComp->GetOwner());
		if (Damagable == nullptr) return;
		if (MinimumDistance < 0.f || Damagable->GetOpponentDistance() >= MinimumDistance)
		{
			Damagable->SpawnProjectile(MeshComp->GetSocketTransform(FireSocketName), DamageConfig);
		}
	}
}
