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
	virtual class UBehaviorTree* GetBehaviorTree() { return nullptr; };
	virtual float GetMovementSpeed() { return 0.f; };
};
