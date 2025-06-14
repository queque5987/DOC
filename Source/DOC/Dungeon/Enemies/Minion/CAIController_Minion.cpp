#include "CAIController_Minion.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IEnemyCharacter.h"

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
		if (LastAction == ActionType)
		{
			ComboStack = (ComboStack + 1) % MaxCombo;
		}
		LastAction = ActionType;

		EnemyCharacter->PlayAnimation(ActionType + ComboStack);
		if (ComboStack == 0 && BlackBoradComponent != nullptr)
		{
			AActor* FaceActor = Cast<AActor>(BlackBoradComponent->GetValueAsObject("PlayerCharacter"));
			if (FaceActor != nullptr)
			{
				EnemyCharacter->SetRotation((FaceActor->GetActorLocation() - EnemyCharacter->GetLocation()).GetSafeNormal().Rotation());
			}
		}
	}
	if (!IsPlayerNear())
	{
		ActionBuffer.Empty();
		LastAction = -1;
		ComboStack = 0;
	}
}

void ACAIController_Minion::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	EnemyCharacter = Cast<IIEnemyCharacter>(InPawn);
	if (EnemyCharacter != nullptr)
	{
		if (BlackBoradComponent != nullptr) BlackBoradComponent->InitializeBlackboard(*EnemyCharacter->GetBehaviorTree()->BlackboardAsset);
		RunBehaviorTree(EnemyCharacter->GetBehaviorTree());
	}
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

bool ACAIController_Minion::IsPlayerNear()
{
	if (BlackBoradComponent != nullptr)
	{
		AActor* tempObject = Cast<AActor>(BlackBoradComponent->GetValueAsObject("PlayerCharacter"));
		if (tempObject != nullptr)
		{
			return FVector::Dist(tempObject->GetActorLocation(), EnemyCharacter->GetLocation()) < 200.f ? true : false;
		}
	}
	return false;
}
