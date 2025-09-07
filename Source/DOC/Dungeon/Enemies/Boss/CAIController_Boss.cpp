#include "CAIController_Boss.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IEnemyCharacter.h"

ACAIController_Boss::ACAIController_Boss(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	BlackBoradComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoradComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	SetPerceptionSystem();
}

void ACAIController_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if ((EnemyCharacter != nullptr && !EnemyCharacter->GetBusy()) &&
		!ActionBuffer.IsEmpty())
	{
		int32 Action;
		ActionBuffer.Dequeue(Action);
		if (Action < 0 && BlackBoradComponent != nullptr)
		{
			bForcedMoveToPlayer = true;
			BlackBoradComponent->SetValueAsBool("bForcedMoveToPlayer", true);
		}
		if (Action >= 0)
		{
			OnEnemyActionDelegatePtr->Broadcast(Action);
			PlayActionCooldown(Action);
		}
	}

	if (bForcedMoveToPlayer && BlackBoradComponent != nullptr)
	{
		FVector PlayerLocation = DetectedPlayer != nullptr ? DetectedPlayer->GetActorLocation() : FVector::ZeroVector;
		FVector CurrentLocation = EnemyCharacter != nullptr ? EnemyCharacter->GetLocation() : FVector::ZeroVector;
		float DistanceFromPlayer = FVector::Dist2D(PlayerLocation, CurrentLocation);
		if (DistanceFromPlayer <= 150.f) BlackBoradComponent->SetValueAsBool("bForcedMoveToPlayer", false);
	}

	if (Curr_Cooldown_Punch < Cooldown_Punch) Curr_Cooldown_Punch += DeltaTime;
	if (Curr_Cooldown_KnockPunch < Cooldown_KnockPunch) Curr_Cooldown_KnockPunch += DeltaTime;
	if (Curr_Cooldown_ComboPunch < Cooldown_ComboPunch) Curr_Cooldown_ComboPunch += DeltaTime;
	if (Curr_Cooldown_JumpCharge < Cooldown_JumpCharge) Curr_Cooldown_JumpCharge += DeltaTime;
	if (Curr_Cooldown_RangedAttack < Cooldown_RangedAttack) Curr_Cooldown_RangedAttack += DeltaTime;
	if (Curr_Cooldown_Charge < Cooldown_Charge) Curr_Cooldown_Charge += DeltaTime;
}

void ACAIController_Boss::OrderAction(int32 ActionType)
{
	UE_LOG(LogTemp, Log, TEXT("ACAIController_Boss::OrderAction"));
	if (EnemyCharacter == nullptr || EnemyCharacter->GetBusy()) return;

	FVector PlayerLocation = DetectedPlayer != nullptr ? DetectedPlayer->GetActorLocation() : FVector::ZeroVector;
	FVector CurrentLocation = EnemyCharacter != nullptr ? EnemyCharacter->GetLocation() : FVector::ZeroVector;
	float DistanceFromPlayer = FVector::Dist2D(PlayerLocation, CurrentLocation);

	if (DistanceFromPlayer <= 250.f)
	{
		if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK))
		{
			ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK);
			return;
		}
		else if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK))
		{
			ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK);
			return;
		}
		else if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK))
		{
			ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK);
			return;
		}
	}
	if (DistanceFromPlayer <= 500.f)
	{
		if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_CHARGE))
		{
			ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_CHARGE);
			return;
		}
	}
	if (DistanceFromPlayer > 500.f)
	{
		if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE))
		{
			ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE);
			return;
		}
		else if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK))
		{
			ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK);
			return;
		}
	}
	ActionBuffer.Enqueue(-1);
}

void ACAIController_Boss::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	EnemyCharacter = Cast<IIEnemyCharacter>(InPawn);
	if (EnemyCharacter != nullptr)
	{
		// AI
		if (BlackBoradComponent != nullptr)
		{
			BlackBoradComponent->InitializeBlackboard(*EnemyCharacter->GetBehaviorTree()->BlackboardAsset);
			BlackBoradComponent->SetValueAsBool("bDead", false);
			BlackBoradComponent->SetValueAsBool("bForcedMoveToPlayer", false);
		}
		RunBehaviorTree(EnemyCharacter->GetBehaviorTree());
		OnEnemyActionDelegatePtr = EnemyCharacter->GetOnEnemyActionDelegate();
	}
}

void ACAIController_Boss::SetPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	if (AIPerceptionComponent != nullptr) SetPerceptionComponent(*AIPerceptionComponent);

	if (SightConfig != nullptr)
	{
		SightConfig->SightRadius = 2000.f;
		SightConfig->LoseSightRadius = 500.f;
		SightConfig->PeripheralVisionAngleDegrees = 320.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 200.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	}
	if (AIPerceptionComponent != nullptr)
	{
		AIPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController_Boss::OnTargetDetected);
		AIPerceptionComponent->ConfigureSense(*SightConfig);
	}
}

void ACAIController_Boss::OnTargetDetected(AActor* actor, FAIStimulus const Stimulus)
{
	if (auto const player = Cast<IIPlayerOnStage>(actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			DetectedPlayer = actor;

			if (BlackBoradComponent != nullptr)
			{
				BlackBoradComponent->SetValueAsVector("PlayerPos", player->GetLocation());
				BlackBoradComponent->SetValueAsObject("PlayerCharacter", actor);
			}
			else UE_LOG(LogTemp, Error, TEXT("ACAIController_Minion : OnTargetDetected No BlackBoard Component Not Found"));
			//if (EnemyCharacter != nullptr)
			//{
			//	EnemyCharacter->SetDealingCharacter(player);
			//}
		}
	}
}

bool ACAIController_Boss::IsActionAvailable(int32 ActionType)
{
	switch (ActionType)
	{
	case(ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK):
		return Curr_Cooldown_Punch >= Cooldown_Punch ? true : false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK):
		return Curr_Cooldown_RangedAttack >= Cooldown_RangedAttack ? true : false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_CHARGE):
		return Curr_Cooldown_Charge >= Cooldown_Charge ? true : false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE):
		return Curr_Cooldown_JumpCharge >= Cooldown_JumpCharge ? true : false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_LIGHT_ATTACK1):
		return false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_LIGHT_ATTACK2):
		return false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK):
		return Curr_Cooldown_ComboPunch >= Cooldown_ComboPunch ? true : false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK):
		return Curr_Cooldown_KnockPunch >= Cooldown_KnockPunch ? true : false;
		break;
	default:
		break;
	}
	return false;
}

void ACAIController_Boss::PlayActionCooldown(int32 ActionType)
{
	switch (ActionType)
	{
	case(ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK):
		Curr_Cooldown_Punch = 0.f;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK):
		Curr_Cooldown_RangedAttack = 0.f;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_CHARGE):
		Curr_Cooldown_Charge = 0.f;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE):
		Curr_Cooldown_JumpCharge = 0.f;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_LIGHT_ATTACK1):
	case(ENEMYCHARACTER_ACTIONTYPE_LIGHT_ATTACK2):
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK):
		Curr_Cooldown_ComboPunch = 0.f;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK):
		Curr_Cooldown_KnockPunch = 0.f;
		break;
	default:
		break;
	}
}
