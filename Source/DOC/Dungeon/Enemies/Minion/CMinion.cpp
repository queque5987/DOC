#include "CMinion.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Dungeon/Enemies/Minion/CAIController_Minion.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/IAnimInstance.h"
#include "GameSystem/CHitBoxComponent.h"
#include "Interfaces/IDamagable.h"
#include "Interfaces/CStageStructs.h"

ACMinion::ACMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh>SkeletalMeshFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Meshes/Minion_Lane_Melee_Core_Dawn.Minion_Lane_Melee_Core_Dawn"));
	ConstructorHelpers::FClassFinder<UAnimInstance>AnimBPFinder(TEXT("/Game/Dungeon/Minion/AnimBP_Minion"));
	if (SkeletalMeshFinder.Succeeded()) GetMesh()->SetSkeletalMesh(SkeletalMeshFinder.Object);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	if (AnimBPFinder.Succeeded()) GetMesh()->SetAnimClass(AnimBPFinder.Class);

	// Animation
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackAFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_A.Attack_A"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackBFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_B.Attack_B"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackCFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_C.Attack_C"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackDFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_D.Attack_D"));
	
	if (AttackAFinder.Succeeded()) AnimSeqArr.Add(AttackAFinder.Object);
	if (AttackBFinder.Succeeded()) AnimSeqArr.Add(AttackBFinder.Object);
	if (AttackCFinder.Succeeded()) AnimSeqArr.Add(AttackCFinder.Object);
	if (AttackDFinder.Succeeded()) AnimSeqArr.Add(AttackDFinder.Object);

	HitBoxComponent = CreateDefaultSubobject<UCHitBoxComponent>(TEXT("HitBoxComponent"));

	// AI
	AIControllerClass = ACAIController_Minion::StaticClass();
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTFinder(TEXT("/Game/Dungeon/Minion/BT_Minion.BT_Minion"));
	if (BTFinder.Succeeded()) BehaviorTree = BTFinder.Object;
	if (GetCharacterMovement() != nullptr) GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

void ACMinion::BeginPlay()
{
	Super::BeginPlay();
	ACAIController_Minion* AICon = GetWorld()->SpawnActor<ACAIController_Minion>(ACAIController_Minion::StaticClass());

	AnimInstance = Cast<IIAnimInstance>(GetMesh()->GetAnimInstance());
	if (AICon != nullptr && AnimInstance != nullptr)
	{
		AICon->Possess(this);
		AICon->SetupDelegates(AnimInstance->GetDelegate_MontagePlayingStateChanged());
	}

}

void ACMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool ACMinion::GetBusy()
{
	return (AnimInstance != nullptr ? AnimInstance->GetBusy() : false);
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

void ACMinion::PlayAnimation(int32 Type)
{
	if (AnimInstance != nullptr) AnimInstance->PlayAnimation(AnimSeqArr[Type]);
}

void ACMinion::ResetTraceProperties()
{
	if (HitBoxComponent != nullptr) HitBoxComponent->ResetProperties();
}

void ACMinion::PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount)
{
	if (HitBoxComponent != nullptr)
	{
		FVector SwingDirection;
		TArray<FHitResult> temp = HitBoxComponent->PerformCapsuleTrace<UIDamagable>(CapsuleRadius, CapsuleHalfHeight, Location, Rotation, Precision, SwingDirection);

		for (FHitResult HitResult : temp)
		{
			if (HitResult.GetActor() != nullptr)
			{
				IIDamagable* Damagable = Cast<IIDamagable>(HitResult.GetActor()); 
				if (Damagable != nullptr)
				{
					FDamageConfig DamageConfig;
					DamageConfig.Instigator = GetController();
					DamageConfig.Damage = DamageAmount;
					DamageConfig.HitDirection = SwingDirection;
					DamageConfig.HitLocation = HitResult.ImpactPoint;
					Damagable->RecieveDamage(DamageConfig);
				}
				UE_LOG(LogTemp, Log, TEXT("ACMinion : PerformCapsuleTrace : %s"), *HitResult.GetActor()->GetName());
			}
		}
	}
}
