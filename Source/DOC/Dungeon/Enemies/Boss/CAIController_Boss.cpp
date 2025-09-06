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
	}
}

void ACAIController_Boss::OrderAction(int32 ActionType)
{
	UE_LOG(LogTemp, Log, TEXT("ACAIController_Boss::OrderAction"));

	FVector PlayerLocation = DetectedPlayer != nullptr ? DetectedPlayer->GetActorLocation() : FVector::ZeroVector;
	FVector CurrentLocation = EnemyCharacter != nullptr ? EnemyCharacter->GetLocation() : FVector::ZeroVector;
	float DistanceFromPlayer = FVector::Dist2D(PlayerLocation, CurrentLocation);
	//ActionBuffer.Enqueue(ActionType);
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
