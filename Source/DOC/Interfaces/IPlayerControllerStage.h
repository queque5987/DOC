#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageStructs.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "IPlayerControllerStage.generated.h"

UINTERFACE(MinimalAPI)
class UIPlayerControllerStage : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIPlayerControllerStage
{
	GENERATED_BODY()

public:
	virtual void SetCounterHitCheck(bool b) {};
	virtual bool GetCounterHitCheck() { return false; };
	virtual void MinimapRemoveBind() {};
	virtual void LightsOutPrevStage() {};
	virtual void SetStage(class IIGeneratedStage* Stage) {};
	virtual void SightReached(FVector& SightLocation) {};
	virtual FVector GetPlayerLocation() { return FVector(); };
	virtual void SetToPerspectiveCamera(FTransform Transform) {};
	virtual void SetToFollowCamera() {};
	virtual void GetUnderCursor(FHitResult& HitResult) {};
	virtual bool RecieveDamage(FDamageConfig DamageConfig) { return bool(); };
	virtual void LockOnMonster(class IIEnemyCharacter* Enemy) {};
	virtual void LockFreeMonster() {};
	virtual bool AttachEquipment(class IIEquipment* ToEquipItem, int32 Type, FName SocketName) { return bool(); };
	virtual class IIObjectPoolManager* GetObjectPoolManager() { return nullptr; };
	virtual FRotator GetCurrentCameraRotation() { return FRotator(); };
	virtual FPlayerStat GetPlayerStat() { return FPlayerStat(); };
	virtual bool DealtDamage(FDamageConfig DamageConfig) { return bool(); };
	virtual FOnChangeCounterReady* GetOnChangeCounterReadyDelegate() { return nullptr; };
	virtual FOutOfMana* GetOutOfManaDelegate() { return nullptr; };
	virtual float GetCurrentMP() { return 0.f; };
	virtual bool TrySpendMP(float e) { return bool(); };
	virtual FPlayerStat* GetPlayerStatPtr() { return nullptr; };
	//virtual void EquipItem(class )
	virtual void SetupDelegates(FOnReceivedDamage* Delegate_OnReceivedDamage, FOnQuickSlotInput* Delegate_OnQuickSlotInput, FPressedKeyboard* Delegate_PressedKeyboard) {};
};
