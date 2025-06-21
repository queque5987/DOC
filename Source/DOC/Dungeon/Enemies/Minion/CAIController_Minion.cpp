#include "CAIController_Minion.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Interfaces/IStageGrid_Room.h"
#include "Interfaces/IAnimInstance.h"
#include "GameFramework/GameStateBase.h"

ACAIController_Minion::ACAIController_Minion(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	BlackBoradComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoradComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	SetPerceptionSystem();
}

void ACAIController_Minion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if ((EnemyCharacter != nullptr && !EnemyCharacter->GetBusy()) &&
		!ActionBuffer.IsEmpty())
	{
		int32 ActionType;
		ActionBuffer.Dequeue(ActionType);
		AActor* FaceActor = Cast<AActor>(BlackBoradComponent->GetValueAsObject("PlayerCharacter"));
		float Radian = FMath::Acos(FVector::DotProduct((FaceActor->GetActorLocation() - EnemyCharacter->GetLocation()).GetSafeNormal(), EnemyCharacter->GetForwardVector()));
		float Deg = FMath::RadiansToDegrees(Radian);

		switch (ActionType)
		{
		case(ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK):
			if (LastAction == ActionType)
			{
				ComboStack = (ComboStack + 1) % MaxCombo;
			}
			LastAction = ActionType;
			EnemyCharacter->PlayAnimation(ComboAttackType + ActionType + ComboStack);
			if (ComboStack == 0 && BlackBoradComponent != nullptr)
			{
				if (FaceActor != nullptr)
				{
					EnemyCharacter->SetRotation((FaceActor->GetActorLocation() - EnemyCharacter->GetLocation()).GetSafeNormal().Rotation());
				}
			}
			break;
		case(ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK):
			if (FaceActor != nullptr && Deg > 45.f)
			{
				EnemyCharacter->PlayAnimation(ENEMYCHARACTER_RANGED_TURN);
			}
			else if (LastAction != ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK)
			{
				if (FaceActor != nullptr)
				{
					EnemyCharacter->PlayAnimation(ENEMYCHARACTER_RANGED_AGGRO);
					//EnemyCharacter->SetRotation((FaceActor->GetActorLocation() - EnemyCharacter->GetLocation()).GetSafeNormal().Rotation());
					LastAction = ActionType;
				}
			}
			else
			{
				if (FaceActor != nullptr)
				{
					EnemyCharacter->PlayAnimation(FMath::Abs(Deg) < 30.f ? ENEMYCHARACTER_RANGED_FIRE : ENEMYCHARACTER_RANGED_AGGRO);
					EnemyCharacter->SetRotation((FaceActor->GetActorLocation() - EnemyCharacter->GetLocation()).GetSafeNormal().Rotation());
					LastAction = ActionType;
				}
			}
			break;
		default:
			break;
		}
	}
	if (EnemyCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ACAIController_Minion : EnemyCharacter Is Nullptr"));
		return;
	}
	if (EnemyCharacter->GetEnemyType() == ENEMYCHARACTER_MINION && !IsPlayerNear(ComboStack <= 0 ? 200.f : 500.f))
	{
		ActionBuffer.Empty();
		LastAction = -1;
		ComboStack = 0;
	}
	//else if (EnemyCharacter->GetEnemyType() == ENEMYCHARACTER_MINION_RANGED && !IsPlayerNear(500.f))
	//{
	//	ActionBuffer.Empty();
	//	LastAction = -1;
	//	ComboStack = 0;
	//}
}

void ACAIController_Minion::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	EnemyCharacter = Cast<IIEnemyCharacter>(InPawn);
	if (EnemyCharacter != nullptr)
	{
		// AI
		if (BlackBoradComponent != nullptr) BlackBoradComponent->InitializeBlackboard(*EnemyCharacter->GetBehaviorTree()->BlackboardAsset);
		RunBehaviorTree(EnemyCharacter->GetBehaviorTree());

		// Attack Type
		ComboAttackType = EnemyCharacter->GetAttackType();
		switch (ComboAttackType)
		{
		case(ENEMYCHARACTER_COMBOATTACK_AA):
			MaxCombo = ENEMYCHARACTER_COMBOATTACK_A_MAX;
			break;
		case(ENEMYCHARACTER_COMBOATTACK_BA):
			MaxCombo = ENEMYCHARACTER_COMBOATTACK_B_MAX;
			break;
		case(ENEMYCHARACTER_COMBOATTACK_CA):
			MaxCombo = ENEMYCHARACTER_COMBOATTACK_C_MAX;
			break;
		default:
			MaxCombo = ENEMYCHARACTER_COMBOATTACK_A_MAX;
			break;
		}

		// AnimInst
		if (EnemyCharacter->GetSKMesh() != nullptr)
		{
			IAnimInst = Cast<IIAnimInstance>(EnemyCharacter->GetSKMesh()->GetAnimInstance());
		}
	}
	//NavSystemManager = Cast<IINavSystemManager>(GetWorld()->GetGameState());
}

void ACAIController_Minion::OnUnPossess()
{
	Super::OnUnPossess();
}

void ACAIController_Minion::SetPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	if (AIPerceptionComponent != nullptr) SetPerceptionComponent(*AIPerceptionComponent);

	if (SightConfig != nullptr)
	{
		SightConfig->SightRadius = 2000.f;
		SightConfig->LoseSightRadius = 500.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 200.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	}
	if (AIPerceptionComponent != nullptr)
	{
		AIPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController_Minion::OnTargetDetected);
		AIPerceptionComponent->ConfigureSense(*SightConfig);
	}
}

void ACAIController_Minion::OnTargetDetected(AActor* actor, FAIStimulus const Stimulus)
{
	if (auto const player = Cast<IIPlayerOnStage>(actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			if (BlackBoradComponent != nullptr)
			{
				BlackBoradComponent->SetValueAsVector("PlayerPos", player->GetLocation());
				BlackBoradComponent->SetValueAsObject("PlayerCharacter", actor);
			}
			else UE_LOG(LogTemp, Error, TEXT("ACAIController_Minion : OnTargetDetected No BlackBoard Component Not Found"));
		}
	}
}

void ACAIController_Minion::OrderAction(int32 ActionType)
{
	ActionBuffer.Enqueue(ActionType);
	UE_LOG(LogTemp, Log, TEXT("ACAIController_Minion : OrderAction : Enqueue Action To Buffer"));
}

void ACAIController_Minion::SetupDelegates(FMONTAGE_PLAYING_STATE_CHANGED* Delegate_MontagePlayingStateChanged)
{
	Delegate_MontagePlayingStateChanged->BindLambda([&](bool newState) {
		if (BlackBoradComponent != nullptr) BlackBoradComponent->SetValueAsBool("Busy", newState);
		}
	);
}

bool ACAIController_Minion::IsPlayerNear(float Distance)
{
	if (BlackBoradComponent != nullptr)
	{
		AActor* tempObject = Cast<AActor>(BlackBoradComponent->GetValueAsObject("PlayerCharacter"));
		if (tempObject != nullptr)
		{
			return FVector::Dist(tempObject->GetActorLocation(), EnemyCharacter->GetLocation()) < Distance ? true : false;
		}
	}
	return false;
}

void ACAIController_Minion::CalculateRangedAttackPosition()
{
	if (BlackBoradComponent == nullptr) return;
	IIStageGrid_Room* Room = EnemyCharacter->GetSpawnedRoom();
	//bSeized = false;
	LastAction = -1;
	ActionBuffer.Empty();
	if (Room != nullptr)
	{
		AActor* tempPlayer = Cast<AActor>(BlackBoradComponent->GetValueAsObject("PlayerCharacter"));
		FVector FirePos;
		if (Room->GetRangedAttackPosition(EnemyCharacter->GetLocation(), tempPlayer->GetActorLocation(), 500.f, 120.f, FirePos))
		{
			BlackBoradComponent->SetValueAsVector("FirePosition", FirePos);
		}
		else
		{
			OrderAction(ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK);
		}
	}
}

AActor* ACAIController_Minion::GetCurrentAttackTargetActor()
{
	if (BlackBoradComponent == nullptr) return nullptr;
	else return Cast<AActor>(BlackBoradComponent->GetValueAsObject("PlayerCharacter"));
}