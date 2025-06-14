#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PCH.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IEnemyCharacter.h"
#include "CMinion.generated.h"

UCLASS()
class DOC_API ACMinion : public ACharacter, public IIInteractableItem, public IIEnemyCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	class UCHitBoxComponent* HitBoxComponent;

public:
	ACMinion();
	
protected:
	class IIAnimInstance* AnimInstance;

	bool Selected;
	virtual void BeginPlay() override;
	TArray<UAnimSequence*> AnimSeqArr;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual bool GetBusy() override;
	virtual FVector GetLocation() override { return GetActorLocation(); };
	virtual void SetRotation(FRotator NewRotation) { SetActorRotation(NewRotation); };
	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	virtual void Select() override;
	virtual void UnSelect() override;

	virtual class UBehaviorTree* GetBehaviorTree() override { return BehaviorTree; };
	virtual float GetMovementSpeed() override { return GetVelocity().Size(); };

	virtual void PlayAnimation(int32 Type) override;

	virtual void ResetTraceProperties() override;
	virtual void PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount) override;
};
