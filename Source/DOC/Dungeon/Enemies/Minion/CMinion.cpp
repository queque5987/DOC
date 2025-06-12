#include "CMinion.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Dungeon/Enemies/Minion/CAIController_Minion.h"

ACMinion::ACMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ACAIController_Minion::StaticClass();
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh>SkeletalMeshFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Meshes/Minion_Lane_Melee_Core_Dawn.Minion_Lane_Melee_Core_Dawn"));
	ConstructorHelpers::FClassFinder<UAnimInstance>AnimBPFinder(TEXT("/Game/Dungeon/Minion/AnimBP_Minion"));
	if (SkeletalMeshFinder.Succeeded()) GetMesh()->SetSkeletalMesh(SkeletalMeshFinder.Object);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	if (AnimBPFinder.Succeeded()) GetMesh()->SetAnimClass(AnimBPFinder.Class);

	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTFinder(TEXT("/Game/Dungeon/Minion/BT_Minion.BT_Minion"));
	if (BTFinder.Succeeded()) BehaviorTree = BTFinder.Object;
}

void ACMinion::BeginPlay()
{
	Super::BeginPlay();
	ACAIController_Minion* AICon = GetWorld()->SpawnActor<ACAIController_Minion>(ACAIController_Minion::StaticClass());

	if (AICon != nullptr) AICon->Possess(this);
}

void ACMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACMinion::Select()
{
	if (Selected) return;
	Selected = true;
	if (GetMesh() != nullptr)
	{
		GetMesh()->SetRenderCustomDepth(true);
		GetMesh()->SetCustomDepthStencilValue(CUSTOMDEPTH_ENEMYCHARACTER);
	}
}

void ACMinion::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (GetMesh() != nullptr)
	{
		GetMesh()->SetRenderCustomDepth(false);
	}
}
