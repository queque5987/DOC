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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;
	bool bBusy;
	FVector PrevRootPos;
	FRotator PrevRootRot;
	float PrevRootZ;
	float RootZStack;
	UAnimSequenceBase* CurrentPlayingAnimation;
	FTimerHandle LastRecieveDamageTimerHandle;
public:
	virtual void SetupDelegates(FOnDeath* InOnDeathDelegate, FOnChangeCounterReady* OnChangeCounterReady, FOnReceivedDamage* InOnReceivedDamageDelegate = nullptr, FOnGroggy* InOnGroggyDelegate = nullptr, FOnGroggyEnd* InOnGroggyEndDeegate = nullptr) override;

	UFUNCTION()
	void OnChangeCounterReady_Callback(bool bReady);

	UPROPERTY(BlueprintReadOnly)
	float MovementSpeed;
	UPROPERTY(BlueprintReadOnly)
	float ForwardSpeed;
	UPROPERTY(BlueprintReadOnly)
	float RightSpeed;
	UPROPERTY(BlueprintReadOnly)
	bool bCounterReady;
	UPROPERTY(BlueprintReadOnly)
	bool bGroggy;
	UPROPERTY(BlueprintReadOnly)
	bool bAirbone;
	UPROPERTY(BlueprintReadOnly)
	bool bDead;

	FVector2D PrevVelocity;

	virtual void PlayAnimation(class UAnimSequenceBase* PlayAnimation, float BlendInTime = 0.25f, float BlendOutTime = 0.25f, float PlayRate = 1.f, float StartTime = 0.f) override;
	virtual void PlayAnimation(class UAnimSequenceBase* PlayAnimation, float BlendInTime = 0.25f, float BlendOutTime = 0.25f, float PlayRate = 1.f, float StartTime = 0.f, FName SlotName = "DefaultSlot") override;

 	virtual bool GetBusy() override { return bBusy; };
	virtual void SetBusy(bool e) override;
	virtual bool GetGroggy() override { return bGroggy; };
	virtual void SetGroggy(bool e) override;
	virtual bool GetAirbone() { return bAirbone; };
	virtual void SetAirbone(bool e) override;
	virtual bool IsMontagePlaying() override;
	virtual void SetCounterReady(bool e) override;
	virtual bool GetCounterReady() override { return bCounterReady; };
	virtual FMONTAGE_PLAYING_STATE_CHANGED* GetDelegate_MontagePlayingStateChanged() { return &Delegate_Montage_Playing_State_Changed; };
	virtual FMONTAGE_PLAYER_COMBO_CLEARED* GetDelegate_MontagePlayerComboCleared() override { return &Delegate_MontagePlayerComboCleared; };
	virtual void SetCurrentMontagePlayRate(float PlayRate) override;
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void ReceiveDamage(FDamageConfig DamageConfig);
	UFUNCTION()
	void OnDeath(FDamageConfig DamageConfig);
	virtual class UAnimSequenceBase* GetCurrentPlayingAnimation() override;
	virtual void StopAnimation() override;
};
