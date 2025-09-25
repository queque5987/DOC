#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interfaces/IEnemyAIController.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Interfaces/CStageStructs.h"
#include "PCH.h"
#include "CAIController_Boss.generated.h"

UCLASS()
class DOC_API ACAIController_Boss : public AAIController, public IIEnemyAIController
{
	GENERATED_BODY()

	ACAIController_Boss(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	class UAISenseConfig_Sight* SightConfig;
	class UAIPerceptionComponent* AIPerceptionComponent;
	class UBlackboardComponent* BlackBoradComponent;
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	class IIEnemyCharacter* EnemyCharacter;

	class AActor* DetectedPlayer;
	TQueue<int32> ActionBuffer;

	FOnEnemyAction* OnEnemyActionDelegatePtr;
	FOnDeath* OnDeathDelegatePtr;

	const float Cooldown_Punch = 1.5f;
	const float Cooldown_KnockPunch = 6.f;
	const float Cooldown_ComboPunch = 7.f;
	const float Cooldown_JumpCharge = 12.f;
	const float Cooldown_RangedAttack = 2.f;
	const float Cooldown_Charge = 6.f;
	const float Cooldown_Uppercut = 5.5f;

	float Curr_Cooldown_Punch = 0.f;
	float Curr_Cooldown_KnockPunch = 0.f;
	float Curr_Cooldown_ComboPunch = 0.f;
	float Curr_Cooldown_JumpCharge = 0.f;
	float Curr_Cooldown_RangedAttack = 0.f;
	float Curr_Cooldown_Charge = 0.f;
	float Curr_Cooldown_Uppercut = 0.f;

	bool bForcedMoveToPlayer = false;
	float ForcedMoveElipsedTime = 0.f;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void OrderAction(int32 ActionType) override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void SetPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus);

	UFUNCTION()
	void Died(FDamageConfig DamageConfig);

private:
	bool IsActionAvailable(int32 ActionType);
	void PlayActionCooldown(int32 ActionType);

public:
	virtual class AActor* GetCurrentAttackTargetActor() override;
	virtual void OverrideNextTickCombo(int32 NextAction, bool bIgnoreCooldown) override;
};
