// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IDamagable.h"
#include "Interfaces/CStageDelegateTypes.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* QuickslotAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShiftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FAction;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* LockedOnParticleSystemComponent;

	UPROPERTY(VisibleAnywhere)
	class UCPlayerGazeComponent* PlayerGazeComponent;

	UPROPERTY(EditAnywhere, Category = "HitBox")
	class UCHitBoxComponent* HitBoxComponent;

	//UPROPERTY(EditAnywhere, Category = "Stat")
	//class UCStatComponent* StatComponent;

	UPROPERTY(EditAnywhere, Category = "Http")
	class UCHttpComponent* HttpComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HairMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HelmetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TorsoMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* GauntletsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BootsMesh;


	FOnEquipmentChanged OnEquipmentChanged;
	FOnReceivedDamage OnReceivedDamage;
public:
	ADOCCharacter();

	UPROPERTY(EditAnywhere)
	int32 TotalSwayLaunchCount = 7;
	UPROPERTY(EditAnywhere)
	float SwayRate = 0.025f;

protected:

	class USoundBase* CounterHitPlaySound;
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void MoveEnd(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	TArray<class UAnimSequence*> AnimSeqArr;
	TArray<class USkeletalMeshComponent*> SkeletalMeshComponents;

	FPlayerTimeSeriesData PlayerTimeSeriesData;
	TDoubleLinkedList<int32> TimeSeriesData_PlayerPressingButton;
	TDoubleLinkedList<FVector> TimeSeriesData_PlayerLocation;
	TDoubleLinkedList<FVector> TimeSeriesData_PlayerForwardVector;
	TDoubleLinkedList<FVector> TimeSeriesData_PlayerVelocityVector;
	TDoubleLinkedList<float> TimeSeriesData_PlayerHP;
	TDoubleLinkedList<float> TimeSeriesData_PlayerStamina;
	TDoubleLinkedList<float> TimeSeriesData_DistFromBottom;
	TDoubleLinkedList<float> TimeSeriesData_DistFromTop;
	TDoubleLinkedList<float> TimeSeriesData_DistFromRight;
	TDoubleLinkedList<float> TimeSeriesData_DistFromLeft;
	//TQueue<FVector> TimeSeriesData_PlayerLocation;
protected:
	void SetSkeletalMeshesCustomDepthStencilValue(int32 StencilValue);

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
	class IIDamagable* ToExecuteMonster;
	FOnChangeCounterReady* OnChangeCounterReadyDelegate;
	FOnQuickSlotInput OnQuickSlotInputDelegate;
	FOnPlayerGroggy* OnGroggyDelegate;
	FOnGroggyEnd* OnGroggyEndDelegate;
	FOnDeath* OnDeathDelegate;
	TSet<class UStaticMeshComponent*> PrevCamBlockingStaticMeshes;
	TSet<class UStaticMeshComponent*> CamBlockingStaticMeshes;

	TMap<int32, class IIEquipment*> EquippedActors;

	int32 TickCounter = 0;
	int32 MaxiumCamBlockingCheck = 16;

	FVector2D MovementVector = FVector2D{0.f, 0.f};
	bool bBusyMontage = false;
	bool bInvincible = false;
	bool bSway = false;
	bool bSwaySucceedBonus = false;
	bool bGroggy = false;
	bool bDead = false;
	int32 LMB_ComboCount = 0;
	int32 RMB_ComboCount = 0;
	float KnockBackTime = 0.f;
	FVector KnockBackDirection;
	FVector DynamicCameraLocation;
	FVector RollDirection;
	int32 LastPlayedAnimSequence;
	FTimerHandle CounterTimerHandle;
	FTimerHandle SwayTimerHandle;
	FTimerHandle InvincibleTimerHandle;
	FTimerHandle SwaySucceedBonusTimerHandle;
	FTimerHandle AirboneTimerHandle;
	int32 CurrentSwayLaunchCount;
	int32 TotalCounterDamageCount;
    int32 CurrentCounterDamageCount;
    TWeakObjectPtr<AActor> StoredCounterDamagable;
    FDamageConfig StoredCounterDamageConfig;
	FDelegateHandle OnDeathLockFreeDelegateHandle;
	FPressedKeyboard OnPressedKeyboard;
	FOnRevive OnRevive;

	PressingButton CurrentPressingButton = PressingButton::None;
	float Dist_from_Top = 0.f;
	float Dist_from_Bottom = 900.f;
	float Dist_from_Left = 0.f;
	float Dist_from_Right = 900.f;

	virtual void SetInvincibleMoment(float SetTime, bool IsSway) override { 
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(InvincibleTimerHandle);
		bInvincible = true;
		if (LockedOnMonster != nullptr && !IsSway) bUseControllerRotationYaw = false;
		if (IsSway) bSway = true;
		TimerManager.SetTimer(InvincibleTimerHandle, FTimerDelegate::CreateLambda([&]() {
			if (LockedOnMonster != nullptr) bUseControllerRotationYaw = true;
			bInvincible = false;
			bSway = false;
			}), SetTime, false);
	}

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	void CorrectCharacterRotation(bool bForcedForward = false);

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
	void Roll();
	void Quickslot(const FInputActionValue& Value);
	void ShiftTriggered();
	void ShiftCompleted();
	void FStarted();
	UFUNCTION()
	void AnyKeyPressed(FKey PressedKey);
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
	virtual bool AttachEquipment(class IIEquipment* ToEquip, int32 Type) override;
	virtual class IIEquipment* DetachEquipment(int32 ItemCode) override;
	virtual FOnEquipmentChanged* GetOnEquipmentChangedDelegate() override { return &OnEquipmentChanged; };
	virtual void CounterAttackSucceeded(FDamageConfig DamageConfig) override;
	virtual FVector GetForwardVector() override { return GetActorForwardVector(); };
	virtual int32 GetCurrentPressingButton() override { return StaticCast<int32>(CurrentPressingButton); };
	virtual void UpdateRoomRelativeLocation(float DistFromTop, float DistFromBottom, float DistFromLeft, float DistFromRight) override;
	virtual void GetRoomRelativeLocation(float& DistFromTop, float& DistFromBottom, float& DistFromLeft, float& DistFromRight) override {
		DistFromTop = Dist_from_Top;
		DistFromBottom = Dist_from_Bottom;
		DistFromLeft = Dist_from_Left;
		DistFromRight = Dist_from_Right;
	};
	virtual FPlayerStat* GetCurrentPlayerStatus() override;
	virtual void SetupDelegates(FOnPlayerGroggy* InDelegate_PlayerGroggyOn, FOnGroggyEnd* InDelegate_PlayerGroggyEnd, FOnDeath* InDelegate_OnDeath) override;
	virtual void DetectedByBoss(class IIDamagable* InBoss) override;
	virtual void CreateTimeSeriesData(FVector EnemyCharacterLocation, FPlayerTimeSeriesDataV2& OutputTimeSeriesData) override;
	virtual void CreateTimeSeriesData(TDoubleLinkedList<FVector>* EnemyCharacterLocationList, FPlayerTimeSeriesDataV2& OutputTimeSeriesData) override;
	/*
		Damage
	*/
	virtual bool RecieveDamage(FDamageConfig DamageConfig) override;
	virtual void ResetTraceProperties() override;
	virtual void PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount) override;
	virtual bool PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, FDamageConfig DamageConfig) override;
	bool DealDamage(IIDamagable* Damagable, FDamageConfig& DamageConfig);
	virtual void Execute(FDamageConfig DamageConfig) override;
	virtual void Catch(float Duration, float PlayRate, FDamageConfig DamageConfig) override;

public:
	UFUNCTION()
	void OnPlayerGroggyOn(FPlayerStat CurrentStat);
	UFUNCTION()
	void OnPlayerGroggyEnd();
	UFUNCTION()
	void OnDeath(FDamageConfig DamageConfig);
};

