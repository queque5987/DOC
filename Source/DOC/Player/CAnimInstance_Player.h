#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/IAnimInstance.h"
#include "CAnimInstance_Player.generated.h"

UCLASS()
class DOC_API UCAnimInstance_Player : public UAnimInstance, public IIAnimInstance
{
	GENERATED_BODY()

	class IIPlayerOnStage* PlayerCharacter;
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;;

	UPROPERTY(BlueprintReadOnly)
	float MovementSpeed;
	UPROPERTY(BlueprintReadOnly)
	float ForwardSpeed;
	UPROPERTY(BlueprintReadOnly)
	float RightSpeed;

	FVector2D PrevVelocity;
};
