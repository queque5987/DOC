#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PCH.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Interfaces/IDamagable.h"
#include "CMinion.generated.h"

UCLASS()
class DOC_API ACMinion : public ACharacter, public IIInteractableItem, public IIEnemyCharacter, public IIDamagable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree_Ranged;

	UPROPERTY(EditAnywhere, Category = "HitBox")
	class UCHitBoxComponent* HitBoxComponent;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UClass* AnimClass_Mellee;
	UPROPERTY(EditAnywhere, Category = "Animation")
	UClass* AnimClass_Ranged;
	UPROPERTY(EditAnywhere, Category = "Animation")
	UClass* AnimClass_Sieze;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	class USkeletalMesh* SKMesh_Mellee;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class USkeletalMesh* SKMesh_Ranged;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class USkeletalMesh* SKMesh_Sieze;
public:
	ACMinion();
	
protected:
	class IIAnimInstance* AnimInstance;
	class IIEnemyAIController* AIController;
	class IIObjectPoolManager* ObjectPoolManager;
	int32 EnemyType;
	bool Selected;
	UPROPERTY(VisibleAnywhere)
	int32 AttackType;
	virtual void BeginPlay() override;
	TArray<TArray<class UAnimSequence*>> AnimSeqArr;
	class IIStageGrid_Room* CurrentSpawnedRoom;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual int32 GetEnemyType() override { return EnemyType; };
	virtual void SetEnemyType(int32 Type) override;
	virtual void SetSpawnedRoom(class IIStageGrid_Room* SpawnedRoom) override { CurrentSpawnedRoom = SpawnedRoom; };
	virtual class IIStageGrid_Room* GetSpawnedRoom() override { return CurrentSpawnedRoom; };
	virtual void SetObjectPoolManager(class IIObjectPoolManager* IOPM) override { ObjectPoolManager = IOPM; };
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual bool GetBusy() override;
	virtual class USkeletalMeshComponent* GetSKMesh() override { return GetMesh(); };
	virtual int32 GetAttackType() override { return AttackType; };
	virtual FVector GetLocation() override { return GetActorLocation(); };
	virtual FVector GetForwardVector() override { return GetActorForwardVector(); };
	virtual FVector GetDealingCharacterLocation() override;
	virtual void SetRotation(FRotator NewRotation) {
		NewRotation.Pitch = 0.f;
		SetActorRotation(NewRotation);
	};
	virtual FRotator GetRotation() override { return GetActorRotation(); };
	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	virtual void Select() override;
	virtual void UnSelect() override;
	virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) override;
	virtual class UBehaviorTree* GetBehaviorTree() override {
		switch (EnemyType)
		{
		case(ENEMYCHARACTER_MINION):
			return BehaviorTree;
			break;
		case(ENEMYCHARACTER_MINION_RANGED):
			return BehaviorTree_Ranged;
			break;
		default:
			break;
		}
		return BehaviorTree;
	};
	virtual float GetMovementSpeed() override { return GetVelocity().Size(); };

	virtual void PlayAnimation(int32 Type) override;

	virtual void ResetTraceProperties() override;
	virtual void PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount) override;

	virtual void SpawnProjectile(FTransform Transform) override;

	/*
		Damage
	*/
	virtual bool RecieveDamage(FDamageConfig DamageConfig) override;
};
