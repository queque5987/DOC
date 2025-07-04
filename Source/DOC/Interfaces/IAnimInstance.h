#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "IAnimInstance.generated.h"

UINTERFACE(MinimalAPI)
class UIAnimInstance : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIAnimInstance
{
	GENERATED_BODY()

public:
	virtual void OnPossess(class IIEnemyCharacter* PossessCharacter) {};
	virtual void PlayAnimation(class UAnimSequenceBase* PlayAnimation) {};
	virtual bool GetBusy() { return bool(); };
	virtual void SetBusy(bool e) {};
	virtual bool IsMontagePlaying() { return bool(); };
	virtual FMONTAGE_PLAYING_STATE_CHANGED* GetDelegate_MontagePlayingStateChanged() { return nullptr; };
	virtual FMONTAGE_PLAYER_COMBO_CLEARED* GetDelegate_MontagePlayerComboCleared() { return nullptr; };
};
