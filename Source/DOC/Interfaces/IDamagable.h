#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageStructs.h"
#include "IDamagable.generated.h"

UINTERFACE(MinimalAPI)
class UIDamagable : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIDamagable
{
	GENERATED_BODY()

public:
	virtual void SpawnProjectile(FTransform Transform) {};
	virtual bool RecieveDamage(FDamageConfig DamageConfig) { return bool(); };
};
