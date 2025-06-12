#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CAnimInstance_Minion.generated.h"

UCLASS()
class DOC_API UCAnimInstance_Minion : public UAnimInstance
{
	GENERATED_BODY()
	
	class IIEnemyCharacter* EnemyCharacter;

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	UPROPERTY(BlueprintReadOnly)
	float MovementSpeed;
};
