#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IEnemyCharacter.generated.h"

UINTERFACE(MinimalAPI)
class UIEnemyCharacter : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIEnemyCharacter
{
	GENERATED_BODY()

public:
	virtual void SetEnemyType(int32 Type) {};
	virtual void SetEnabled(bool e) {};
	virtual bool GetBusy() { return bool(); };
	virtual FVector GetLocation() { return FVector(); };
	virtual void SetRotation(FRotator NewRotation) {};
	virtual class UBehaviorTree* GetBehaviorTree() { return nullptr; };
	virtual float GetMovementSpeed() { return 0.f; };
	virtual void PlayAnimation(int32 Type) {};
	virtual void ResetTraceProperties() {};
	virtual void PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount) {};
};
