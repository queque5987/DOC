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

	FOnDeath* OnDeathDele;
	FDelegateHandle OnDeathDelegateHandle;
	FOnReceivedDamage* OnReceivedDamageDele;
	FOnGroggy* OnGroggyDelegate;
	FOnGroggyEnd* OnGroggyEndDelegate;
	FDelegateHandle OnReceivedDamage_Callback_Handle;
	FDelegateHandle OnGroggy_Callback_Handle;
	FDelegateHandle OnGroggyEnd_Callback_Handle;

	FTimerHandle LastRecieveDamageTimerHandle;
public:
	virtual void SetupDelegates(FOnChangeCounterReady* OnChangeCounterReady, FOnReceivedDamage* InOnReceivedDamageDelegate, FOnGroggy* InOnGroggyDelegate, FOnGroggyEnd* InOnGroggyEndDeegate) override;
public:
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
	virtual FMONTAGE_PLAYING_STATE_CHANGED* GetDelegate_MontagePlayingStateChanged() override { return &Delegate_Montage_Playing_State_Changed; };
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
