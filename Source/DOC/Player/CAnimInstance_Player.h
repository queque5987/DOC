#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/IAnimInstance.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CAnimInstance_Player.generated.h"

UCLASS()
class DOC_API UCAnimInstance_Player : public UAnimInstance, public IIAnimInstance
{
	GENERATED_BODY()

	class IIPlayerOnStage* PlayerCharacter;
	FMONTAGE_PLAYING_STATE_CHANGED Delegate_Montage_Playing_State_Changed;
	FMONTAGE_PLAYER_COMBO_CLEARED Delegate_MontagePlayerComboCleared;
	bool bBusy;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;
	FVector PrevRootPos;
	FRotator PrevRootRot;
	float PrevRootZ;
	float RootZStack;
public:

	UPROPERTY(BlueprintReadOnly)
	float MovementSpeed;
	UPROPERTY(BlueprintReadOnly)
	float ForwardSpeed;
	UPROPERTY(BlueprintReadOnly)
	float RightSpeed;
	UPROPERTY(BlueprintReadOnly)
	bool bCounterReady;

	FVector2D PrevVelocity;

	virtual void PlayAnimation(class UAnimSequenceBase* PlayAnimation, float BlendInTime = 0.25f, float BlendOutTime = 0.25f, float PlayRate = 1.f) override;

	virtual bool GetBusy() override { return bBusy; };
	virtual void SetBusy(bool e) override;
	virtual bool IsMontagePlaying() override;
	virtual void SetCounterReady(bool e) override;
	virtual bool GetCounterReady() override { return bCounterReady; };
	virtual FMONTAGE_PLAYING_STATE_CHANGED* GetDelegate_MontagePlayingStateChanged() { return &Delegate_Montage_Playing_State_Changed; };
	virtual FMONTAGE_PLAYER_COMBO_CLEARED* GetDelegate_MontagePlayerComboCleared() override { return &Delegate_MontagePlayerComboCleared; };
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);
};
