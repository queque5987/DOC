#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageStructs.h"
#include "Interfaces/CStageDelegateTypes.h"
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
	virtual void ResetTraceProperties() {};
	virtual void PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount) {};
	virtual bool PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, FDamageConfig DamageConfig) { return bool(); };
	virtual FHP_CHANGED* GetHPChangedDelegate() { return nullptr; };
	virtual void Died(FDamageConfig DamageConfig) {};
	virtual void Groggy(FDamageConfig DamageConfig) {};
	virtual void Execute(FDamageConfig DamageConfig) {};
	virtual bool IsExecutable() { return bool(); };
	virtual void Catch(float Duration, float PlayRate, FDamageConfig DamageConfig) {};
	virtual void Stun(float Duration, FDamageConfig DamageConfig) {};
	virtual void SetInvincibleMoment(float SetTime, bool IsSway) {};
};
