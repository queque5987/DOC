#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PCH.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Interfaces/IDamagable.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Interfaces/CStageStructs.h"
#include "CBoss.generated.h"

UCLASS()
class DOC_API ACBoss : public ACharacter, public IIInteractableItem, public IIEnemyCharacter, public IIDamagable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "AI")
	FVector SplineTangent0;
	UPROPERTY(EditAnywhere, Category = "AI")
	FVector SplineTangent1;
	UPROPERTY(EditAnywhere, Category = "AI")
	FVector SplineTangent2;
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	class UCHitBoxComponent* HitBoxComponent;
	UPROPERTY(EditAnywhere, Category = "Stat")
	class UCStatComponent* StatComponent;
	UPROPERTY(EditAnywhere, Category = "Animation")
	UClass* AnimClass_Boss;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class UCMonsterHP* MonsterHPComponent;
	UPROPERTY(EditAnywhere, Category = "FX")
	class UNiagaraComponent* OnDeathNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Spline")
	class USplineComponent* SplineComponent;

	TArray<class UAnimSequence*> AnimSeqArr;
public:
	ACBoss();

protected:
	class IIAnimInstance* AnimInstance;
	class IIEnemyAIController* AIController;
	class IIObjectPoolManager* ObjectPoolManager;
	class IIPlayerOnStage* PlayerCharacter;
	class IIStageGrid_Room* CurrentSpawnedRoom;

	FOnDeath MinionDiedCompletedDelegate;
	FOnReceivedDamage OnReceivedDamageDelegate;
	FOnEnemyAction OnEnemyActionDelegate;
	bool Selected;

	virtual void BeginPlay() override;
public:
	// Interactable //

	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	virtual void Select() override;
	virtual void UnSelect() override;
	virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) override;

	virtual void SetEnabled(bool e) override;
	//virtual int32 GetEnemyType() override { return EnemyType; };
	//virtual void SetEnemyType(int32 Type) override;
	//virtual void SetSpawnedRoom(class IIStageGrid_Room* SpawnedRoom) override { CurrentSpawnedRoom = SpawnedRoom; };
	//virtual class IIStageGrid_Room* GetSpawnedRoom() override { return CurrentSpawnedRoom; };
	//virtual void SetObjectPoolManager(class IIObjectPoolManager* IOPM) override { ObjectPoolManager = IOPM; };
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual bool GetBusy() override;
	//virtual class USkeletalMeshComponent* GetSKMesh() override { return GetMesh(); };
	//virtual int32 GetAttackType() override { return AttackType; };
	virtual FVector GetLocation() override { return GetActorLocation(); };
	virtual FVector GetForwardVector() override { return GetActorForwardVector(); };
	virtual FVector GetDealingCharacterLocation() override;
	//virtual void SetDealingCharacter(class IIPlayerOnStage* DealingCharacter) override;
	virtual void SetRotation(FRotator NewRotation) {
		NewRotation.Pitch = 0.f;
		SetActorRotation(NewRotation);
	};
	virtual FRotator GetRotation() override { return GetActorRotation(); };
	//virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	//virtual void Select() override;
	//virtual void UnSelect() override;
	//virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) override;
	virtual class UBehaviorTree* GetBehaviorTree() override { return BehaviorTree; };
	//	switch (EnemyType)
	//	{
	//	case(ENEMYCHARACTER_MINION):
	//		return BehaviorTree;
	//		break;
	//	case(ENEMYCHARACTER_MINION_RANGED):
	//		return BehaviorTree_Ranged;
	//		break;
	//	default:
	//		break;
	//	}
	//	return BehaviorTree;
	//};
	//virtual float GetMovementSpeed() override { return GetVelocity().Size(); };

	//virtual void PlayAnimation(int32 Type) override;

	//virtual void ResetTraceProperties() override;
	//virtual void PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount) override;

	//virtual void SpawnProjectile(FTransform Transform) override;
	virtual void SpawnProjectile(FTransform Transform, FDamageConfig DamageConfig) override;
	virtual FOnDeath* GetOnDeathDelegate() override { return &MinionDiedCompletedDelegate; };
	//virtual FOnDeath* GetOnDiedCompletedDelegate() override;
	virtual FOnEnemyAction* GetOnEnemyActionDelegate() override { return &OnEnemyActionDelegate; };
	//virtual void PlayDiedFX(int32 FXSequence) override;
	//virtual class UAnimSequence* GetHitReactAnimSequence(int32 HitDirection) override;
		virtual FTransform GetSplineTransformAtTime(float Time) override;
	///*
	//	Damage
	//*/
	//virtual bool RecieveDamage(FDamageConfig DamageConfig) override;
	virtual void ResetTraceProperties() override;
	virtual bool PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, FDamageConfig DamageConfig) override;
	UFUNCTION()
	virtual void Died(FDamageConfig DamageConfig) override;
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnEnemyAction(int32 ActionType);
};
