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

protected:
	class IIEnemyCharacter* EnemyCharacter;
	FMONTAGE_PLAYING_STATE_CHANGED Delegate_Montage_Playing_State_Changed;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	UAnimSequenceBase* CurrentPlayingAnimation;

	FOnDeath* OnDeathDele;
	FDelegateHandle OnDeathDelegateHandle;
	FOnReceivedDamage* OnReceivedDamageDele;
	FOnGroggy* OnGroggyDelegate;
	FOnGroggyEnd* OnGroggyEndDelegate;
	FDelegateHandle OnReceivedDamage_Callback_Handle;
	FDelegateHandle OnGroggy_Callback_Handle;
	FDelegateHandle OnGroggyEnd_Callback_Handle;
	FDelegateHandle OnDeath_Callback_Handle;
	FDelegateHandle OnDeathComplete_Callback_Handle;

	FTimerHandle LastRecieveDamageTimerHandle;
public:
	virtual void SetupDelegates(FOnChangeCounterReady* OnChangeCounterReady, FOnReceivedDamage* InOnReceivedDamageDelegate, FOnGroggy* InOnGroggyDelegate, FOnGroggyEnd* InOnGroggyEndDeegate) override;
	virtual class UAnimSequenceBase* GetCurrentPlayingAnimation() override;
public:
	UPROPERTY(BlueprintReadOnly)
	bool bBusy;
	UPROPERTY(BlueprintReadOnly)
	bool bGroggy = false;
	UPROPERTY(BlueprintReadOnly)
	bool Deceased = false;
	UPROPERTY(BlueprintReadOnly)
	bool bEnableRigidBody = false;
	UPROPERTY(BlueprintReadOnly)
	float MovementSpeed;
	virtual void OnPossess(class IIEnemyCharacter* PossessCharacter) override;
	virtual void PlayAnimation(class UAnimSequenceBase* PlayAnimation, float BlendInTime = 0.25f, float BlendOutTime = 0.25f, float PlayRate = 1.f, float StartTime = 0.f) override;
	virtual void SetBusy(bool e) override;
	virtual bool GetBusy() override;
	virtual bool GetGroggy() override { return bGroggy; };
	virtual void SetGroggy(bool e) override { bGroggy = e; };
	virtual FMONTAGE_PLAYING_STATE_CHANGED* GetDelegate_MontagePlayingStateChanged() override { return &Delegate_Montage_Playing_State_Changed; };
	virtual void StopAnimation() override;
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	virtual void Died(FDamageConfig DamageConfig) override;
	UFUNCTION()
	void OnGroggy(FDamageConfig DamageConfig);
	UFUNCTION()
	void OnReceivedDamage(FDamageConfig DamageConfig);
	UFUNCTION()
	void OnGroggyEnd();
};
