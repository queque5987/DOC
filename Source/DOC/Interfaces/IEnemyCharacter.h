#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CstageDelegateTypes.h"
#include "IEnemyCharacter.generated.h"

UINTERFACE(MinimalAPI)
class UIEnemyCharacter : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIEnemyCharacter
{
	GENERATED_BODY()

public:
	virtual int32 GetEnemyType() { return int32(); };
	virtual void SetEnemyType(int32 Type) {};
	virtual void SetSpawnedRoom(class IIStageGrid_Room* SpawnedRoom) {};
	virtual void SetObjectPoolManager(class IIObjectPoolManager* IOPM) {};
	virtual class IIStageGrid_Room* GetSpawnedRoom() { return nullptr; };
	virtual void SetEnabled(bool e) {};
	virtual bool GetBusy() { return bool(); };
	virtual class USkeletalMeshComponent* GetSKMesh() { return nullptr; };
	virtual int32 GetAttackType() { return int32(); };
	virtual FVector GetLocation() { return FVector(); };
	virtual FVector GetForwardVector() { return FVector(); };
	virtual FVector GetDealingCharacterLocation() { return FVector(); };
	virtual void SetRotation(FRotator NewRotation) {};
	virtual FRotator GetRotation() { return FRotator(); };
	virtual class UBehaviorTree* GetBehaviorTree() { return nullptr; };
	virtual float GetMovementSpeed() { return 0.f; };
	virtual void PlayAnimation(int32 Type) {};
	virtual void SetDealingCharacter(class IIPlayerOnStage* DealingCharacter) {};
	//virtual void ResetTraceProperties() {};
	//virtual void PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount) {};
	virtual FOnDeath* GetOnDiedCompletedDelegate() { return nullptr; };
	virtual FOnDeath* GetOnDeathDelegate() { return nullptr; };
	virtual FOnGroggy* GetOnGroggyDelegate() { return nullptr; };
	virtual FOnGroggyEnd* GetOnGroggyEndDelegate() { return nullptr; };
	virtual void PlayRagdoll() {};
	virtual void PlayDiedFX(int32 FXSequence) {};
	virtual class UAnimSequence* GetHitReactAnimSequence(int32 HitDirection) { return nullptr; };
	virtual FString GetEnemyName() { return FString(TEXT("DefaultEnemyName")); };
	virtual float GetExp() { return 0.f; };
};
