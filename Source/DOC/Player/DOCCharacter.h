// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IDamagable.h"
#include "PCH.h"
#include "DOCCharacter.generated.h"


UCLASS(config=Game)
class ADOCCharacter : public ACharacter, public IIPlayerOnStage, public IIDamagable
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PerspectiveCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WidemapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LMBAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RMBAction;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* LockedOnParticleSystemComponent;

	UPROPERTY(VisibleAnywhere)
	class UCPlayerGazeComponent* PlayerGazeComponent;

	UPROPERTY(EditAnywhere, Category = "HitBox")
	class UCHitBoxComponent* HitBoxComponent;

	UPROPERTY(EditAnywhere, Category = "Stat")
	class UCStatComponent* StatComponent;
public:
	ADOCCharacter();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	TArray<class UAnimSequence*> AnimSeqArr;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void StopJumping() override;
	virtual void Tick(float DeltaSeconds) override;

	class IIPlayerControllerStage* IPCS;
	class IIPlayerControllerUI* IPCUI;
	class IIInteractableItem* InteractableItem;
	class IIEnemyCharacter* LockedOnMonster;
	class IIObjectPoolManager* ObjectPoolManager;
	class IIAnimInstance* AnimInstance;

	TSet<class UStaticMeshComponent*> PrevCamBlockingStaticMeshes;
	TSet<class UStaticMeshComponent*> CamBlockingStaticMeshes;

	int32 TickCounter = 0;
	int32 MaxiumCamBlockingCheck = 16;

	FVector2D MovementVector;
	bool bBusyMontage = false;
	int32 LMB_ComboCount = 0;
	int32 RMB_ComboCount = 0;
	FVector DynamicCameraLocation;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/*Deprecated*/
	virtual class IIPlayerControllerStage* GetPlayerControllerStage() override;
	virtual class UObject* GetControllerAsObject() override;
	virtual FVector GetLocation() override { return GetActorLocation(); };
	virtual void SetToPerspectiveCamera(FTransform Transform) override;
	virtual void SetToFollowCamera() override;
	void Interact();
	void ToggleInventory();
	void TurnOnWidemap();
	void TurnOffWidemap();
	void LMB();
	void RMB();
	virtual void LockOnMonster(class IIEnemyCharacter* Enemy) override;
	virtual void LockFreeMonster() override;
	virtual void Controller_SetControlRotation(FRotator Rotation) override { 
		if (GetController() != nullptr) GetController()->SetControlRotation(Rotation);
	};
	virtual FRotator GetRotation() override { return GetActorRotation(); };
	virtual FVector GetUpVector() override { return GetActorUpVector(); };
	virtual FVector2D GetMovementVector() override { return MovementVector; };
	virtual FVector GetPlayerVelocity() override { return GetVelocity(); };
	virtual FTransform GetCameraTransform() override;
	virtual void AdjustRootBone(FVector AdjustVector, bool bLaunch, bool bAllowReverse) override;
	virtual void AdjustMeshRotation(FRotator AdjustRotator) override;
	virtual void AdjustMesh(FVector VerticalVector, FRotator AdjustRotator, FVector LaunchVector) override;

	virtual bool AttachEquipment(class AActor* ToAttachActor, int32 Type, FName SocketName) override;

	/*
		Damage
	*/
	virtual bool RecieveDamage(FDamageConfig DamageConfig) override;
	virtual void ResetTraceProperties() override;
	virtual void PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount) override;
};

