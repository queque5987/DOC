#pragma once

#include "CoreMinimal.h"
#include "Dungeon/Enemies/Minion/CAnimInstance_Minion.h"
#include "CAnimInstance_Boss.generated.h"

UCLASS()
class DOC_API UCAnimInstance_Boss : public UCAnimInstance_Minion
{
	GENERATED_BODY()
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FOnEnemyAction* OnEnemyActionDelegatePtr;

public:
	UPROPERTY(BlueprintReadOnly)
	FVector MovementDirection;
	UPROPERTY(BlueprintReadOnly)
	float fMovementDirection;

	virtual FVector GetMovementDirectionVector() override;
};
