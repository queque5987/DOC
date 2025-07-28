#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/IAnimInstance.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CAnimInstance_Minion.generated.h"

UCLASS()
class DOC_API UCAnimInstance_Minion : public UAnimInstance, public IIAnimInstance
{
	GENERATED_BODY()
	
	class IIEnemyCharacter* EnemyCharacter;
	FMONTAGE_PLAYING_STATE_CHANGED Delegate_Montage_Playing_State_Changed;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	bool bBusy;
public:
	UPROPERTY(BlueprintReadOnly)
	float MovementSpeed;
	virtual void OnPossess(class IIEnemyCharacter* PossessCharacter) override;
	virtual void PlayAnimation(class UAnimSequenceBase* PlayAnimation, float BlendInTime = 0.25f, float BlendOutTime = 0.25f, float PlayRate = 1.f) override;
	virtual void SetBusy(bool e) override;
	virtual bool GetBusy() override;
	virtual FMONTAGE_PLAYING_STATE_CHANGED* GetDelegate_MontagePlayingStateChanged() override { return &Delegate_Montage_Playing_State_Changed; };
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);

};
