#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Interfaces/CStageStructs.h"
#include "IPlayerOnStage.generated.h"

UINTERFACE(MinimalAPI)
class UIPlayerOnStage : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIPlayerOnStage
{
	GENERATED_BODY()

public:
	virtual class IIPlayerControllerStage* GetPlayerControllerStage() { return nullptr; };
	virtual FVector GetLocation() { return FVector(); };
	virtual class UObject* GetControllerAsObject() { return nullptr; };
	virtual void MinimapRemoveDynamic() {};
	virtual void SetToPerspectiveCamera(FTransform Transform) {};
	virtual void SetToFollowCamera() {};
	virtual void LockOnMonster(class IIEnemyCharacter* Enemy) {};
	virtual void LockFreeMonster() {};
	virtual void Controller_SetControlRotation(FRotator Rotation) {};
	virtual FRotator GetRotation() { return FRotator(); };
	virtual FVector GetUpVector() { return FVector(); };
	virtual FVector2D GetMovementVector() { return FVector2D(); };
	virtual FVector GetPlayerVelocity() { return FVector(); };
	virtual FVector GetForwardVector() { return FVector(); };
	virtual FTransform GetCameraTransform() { return FTransform(); };
	virtual void AdjustRootBone(FVector AdjustVector, bool bLaunch, bool bAllowReverse) {};
	virtual void AdjustMeshRotation(FRotator AdjustRotator) {};
	virtual void AdjustMesh(FVector VerticalVector, FRotator AdjustRotator, FVector LaunchVector) {};

	virtual bool AttachEquipment(class AActor* ToAttachActor, int32 Type, FName SocketName) { return bool(); };
	virtual bool AttachEquipment(class IIEquipment* ToEquip, int32 Type) { return bool(); };
	virtual class IIEquipment* DetachEquipment(int32 ItemCode) { return nullptr; };

	virtual FOnEquipmentChanged* GetOnEquipmentChangedDelegate() { return nullptr; };
	virtual FOnRevive* GetOnReviveDelegate() { return nullptr; };
	virtual void CounterAttackSucceeded(FDamageConfig DamageConfig) {};

	virtual int32 GetCurrentPressingButton() { return int32(); };
	virtual void UpdateRoomRelativeLocation(float DistFromTop, float DistFromBottom, float DistFromLeft, float DistFromRight) {};
	virtual void GetRoomRelativeLocation(float& DistFromTop, float& DistFromBottom, float& DistFromLeft, float& DistFromRight) {};
	virtual FPlayerStat* GetCurrentPlayerStatus() { return nullptr; }

	virtual void SetupDelegates(FOnPlayerGroggy* InDelegate_PlayerGroggyOn, FOnGroggyEnd* InDelegate_PlayerGroggyEnd, FOnDeath* InDelegate_OnDeath) {};
	//virtual void SetupDelegates(FOnDeath* InDelegate_BossDied) {};
	virtual void DetectedByBoss(class IIDamagable* InBoss) {};

	virtual void CreateTimeSeriesData(FVector EnemyCharacterLocation, FPlayerTimeSeriesDataV2& OutputTimeSeriesData) {};
	virtual void CreateTimeSeriesData(TDoubleLinkedList<FVector>* EnemyCharacterLocationList, FPlayerTimeSeriesDataV2& OutputTimeSeriesData) {};
	virtual void CreateTimeSeriesData(FVector EnemyCharacterLocation, TArray<FPlayerTimeSeriesDataV3>& OutTimeSeriesData) {};
};
