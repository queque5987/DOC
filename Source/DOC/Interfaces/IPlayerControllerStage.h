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
	//virtual void EquipItem(class )
	//virtual void SetupDelegates(FMONTAGE_PLAYING_STATE_CHANGED* Delegate_MontagePlayingStateChanged) override {};
};
