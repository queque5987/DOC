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

public:
	ACMinion();

protected:
	bool Selected;
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	virtual void Select() override;
	virtual void UnSelect() override;

	virtual class UBehaviorTree* GetBehaviorTree() override { return BehaviorTree; };
	virtual float GetMovementSpeed() override { return GetVelocity().Size(); };
};
