#include "CAIController_Boss.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Interfaces/IDamagable.h"

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
			ForcedMoveElipsedTime = 0.f;
		}
		if (Action >= 0)
		{
			if (EnemyCharacter != nullptr && DetectedPlayer != nullptr)
			{
				float rtn = EnemyCharacter->InferencePlayerNextMove(Cast<IIPlayerOnStage>(DetectedPlayer));
				int32 InferencedMove = FMath::RoundToInt32(rtn);
				UE_LOG(LogTemp, Log, TEXT("Inferenced Move : %d"), InferencedMove);
				FVector InferencedVector = DetectedPlayer->GetActorForwardVector();
				InferencedVector.Z = 0.f;
				switch (InferencedMove % 100)
				{
				case(PressingButton::Forward):
					InferencedVector = EnemyCharacter->GetForwardVector();
					break;
				case(PressingButton::ForwardLeft):
					InferencedVector = EnemyCharacter->GetForwardVector() - EnemyCharacter->GetRightVector();
					break;
				case(PressingButton::Left):
					InferencedVector = -EnemyCharacter->GetRightVector();
					break;
				case(PressingButton::BackLeft):
					InferencedVector = -(EnemyCharacter->GetForwardVector() + EnemyCharacter->GetRightVector());
					break;
				case(PressingButton::Back):
					InferencedVector = -EnemyCharacter->GetForwardVector();
					break;
				case(PressingButton::BackRight):
					InferencedVector = EnemyCharacter->GetRightVector() - EnemyCharacter->GetForwardVector();
					break;
				case(PressingButton::Right):
					InferencedVector = EnemyCharacter->GetRightVector();
					break;
				case(PressingButton::ForwardRight):
					InferencedVector = EnemyCharacter->GetRightVector() + EnemyCharacter->GetForwardVector();
					break;
				default:
					break;
				}
				InferencedVector.Normalize();

				DrawDebugDirectionalArrow(
					GetWorld(), DetectedPlayer->GetActorLocation(), DetectedPlayer->GetActorLocation() + InferencedVector * 300.f, 30.f, FColor::Red, false, 3.f
				);
			}

			OnEnemyActionDelegatePtr->Broadcast(Action);
			PlayActionCooldown(Action);
		}
	}

	if (bForcedMoveToPlayer && BlackBoradComponent != nullptr)
	{
		ForcedMoveElipsedTime += DeltaTime;
		FVector PlayerLocation = DetectedPlayer != nullptr ? DetectedPlayer->GetActorLocation() : FVector::ZeroVector;
		FVector CurrentLocation = EnemyCharacter != nullptr ? EnemyCharacter->GetLocation() : FVector::ZeroVector;
		float DistanceFromPlayer = FVector::Dist2D(PlayerLocation, CurrentLocation);
		if (ForcedMoveElipsedTime >= 3.f)
		{
			OverrideNextTickCombo(ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE, true);
			bForcedMoveToPlayer = false;
			BlackBoradComponent->SetValueAsBool("bForcedMoveToPlayer", false);
			ForcedMoveElipsedTime = 0.f;
		}
		else if (DistanceFromPlayer <= 150.f)
		{
			bForcedMoveToPlayer = false;
			BlackBoradComponent->SetValueAsBool("bForcedMoveToPlayer", false);
		}
		else
		{
			if ((EnemyCharacter != nullptr && !EnemyCharacter->GetBusy()))
			{
				FVector ForwardVector = EnemyCharacter->GetForwardVector();
				FVector BossPlayerVector = (PlayerLocation - CurrentLocation).GetSafeNormal2D();
				FRotator NewRotation;
				float DegDeff = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector, BossPlayerVector)));
				if (DegDeff < 5.f) NewRotation = BossPlayerVector.Rotation();
				else
				{
					FVector Axis = FVector::CrossProduct(ForwardVector, BossPlayerVector).GetSafeNormal();
					FVector NewVector = ForwardVector.RotateAngleAxis(5.f, Axis);
					NewRotation = NewVector.Rotation();
				}
				EnemyCharacter->ManualMoveToDirection(ForwardVector);
				EnemyCharacter->SetRotation(NewRotation);
			}
		}
	}

	if (Curr_Cooldown_Punch < Cooldown_Punch) Curr_Cooldown_Punch += DeltaTime;
	if (Curr_Cooldown_KnockPunch < Cooldown_KnockPunch) Curr_Cooldown_KnockPunch += DeltaTime;
	if (Curr_Cooldown_ComboPunch < Cooldown_ComboPunch) Curr_Cooldown_ComboPunch += DeltaTime;
	if (Curr_Cooldown_JumpCharge < Cooldown_JumpCharge) Curr_Cooldown_JumpCharge += DeltaTime;
	if (Curr_Cooldown_RangedAttack < Cooldown_RangedAttack) Curr_Cooldown_RangedAttack += DeltaTime;
	if (Curr_Cooldown_Charge < Cooldown_Charge) Curr_Cooldown_Charge += DeltaTime;
	if (Curr_Cooldown_Uppercut < Cooldown_Uppercut) Curr_Cooldown_Uppercut += DeltaTime;
}

void ACAIController_Boss::OrderAction(int32 ActionType)
{
	UE_LOG(LogTemp, Log, TEXT("ACAIController_Boss::OrderAction"));
	if (EnemyCharacter == nullptr || EnemyCharacter->GetBusy() || EnemyCharacter->IsDead()) return;

	FVector PlayerLocation = DetectedPlayer != nullptr ? DetectedPlayer->GetActorLocation() : FVector::ZeroVector;
	FVector CurrentLocation = EnemyCharacter != nullptr ? EnemyCharacter->GetLocation() : FVector::ZeroVector;
	float DistanceFromPlayer = FVector::Dist2D(PlayerLocation, CurrentLocation);
	FVector E_P_Vector = (PlayerLocation - CurrentLocation).GetSafeNormal2D();
	FVector ForwardVector = (EnemyCharacter != nullptr ? EnemyCharacter->GetForwardVector() : FVector::ZeroVector).GetSafeNormal2D();
	float Deg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(E_P_Vector, ForwardVector)));
	UE_LOG(LogTemp, Log, TEXT("Deg : %f"), Deg);

	if (FMath::Abs(Deg) > 30.f)
	{
		ActionBuffer.Enqueue(Deg < 0.f ? ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_L : ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_R);
		return;
	}
	if (DistanceFromPlayer <= 500.f)
	{
		if (DistanceFromPlayer <= 250.f)
		{
			if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK))
			{
				ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK);
				return;
			}
			else if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_UPPERCUT))
			{
				ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_UPPERCUT);
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
		else // 250.f < DistanceFromPlayer <= 500.f
		{
			if (IsActionAvailable(ENEMYCHARACTER_ACTIONTYPE_CHARGE))
			{
				ActionBuffer.Enqueue(ENEMYCHARACTER_ACTIONTYPE_CHARGE);
				return;
			}
		}
	}
	if (DistanceFromPlayer <= 1400.f)
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
		OnDeathDelegatePtr = EnemyCharacter->GetOnDeathDelegate();
		if (OnDeathDelegatePtr != nullptr)
		{
			OnDeathDelegatePtr->AddUFunction(this, TEXT("Died"));
		}
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
			player->DetectedByBoss(Cast<IIDamagable>(EnemyCharacter));
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

void ACAIController_Boss::Died(FDamageConfig DamageConfig)
{
	ActionBuffer.Empty();
	if (BlackBoradComponent != nullptr) BlackBoradComponent->SetValueAsBool("bDead", true);
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
	case(ENEMYCHARACTER_ACTIONTYPE_UPPERCUT):
		return Curr_Cooldown_Uppercut >= Cooldown_Uppercut ? true : false;
		return false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK2):
		return false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK):
		return Curr_Cooldown_ComboPunch >= Cooldown_ComboPunch ? true : false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK):
		return Curr_Cooldown_KnockPunch >= Cooldown_KnockPunch ? true : false;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_L):
		return true;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_R):
		return true;
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
	case(ENEMYCHARACTER_ACTIONTYPE_UPPERCUT):
		Curr_Cooldown_Uppercut = 0.f;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK2):
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK):
		Curr_Cooldown_ComboPunch = 0.f;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK):
		Curr_Cooldown_KnockPunch = 0.f;
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_L):
		break;
	case(ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_R):
		break;
	default:
		break;
	}
}

AActor* ACAIController_Boss::GetCurrentAttackTargetActor()
{
	return DetectedPlayer;
}

void ACAIController_Boss::OverrideNextTickCombo(int32 NextAction, bool bIgnoreCooldown)
{
	if (bIgnoreCooldown || (!bIgnoreCooldown && IsActionAvailable(NextAction)))
	{
		PlayActionCooldown(NextAction);
		ActionBuffer.Empty();
		ActionBuffer.Enqueue(NextAction);
	}
}
