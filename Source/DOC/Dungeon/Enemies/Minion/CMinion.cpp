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
	AnimSeqArr.Reserve(ENEMYCHARACTER_COMBOATTACK_NUM);

	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackAAFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_A.Attack_A"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackABFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_B.Attack_B"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackACFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_C.Attack_C"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackADFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_D.Attack_D"));
	
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackBAFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_A_SetA.Attack_A_SetA"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackBBFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_B_SetA.Attack_B_SetA"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackBCFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_C_SetA.Attack_C_SetA"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackBDFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_D_SetA.Attack_D_SetA"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackBEFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_E_SetA.Attack_E_SetA"));
	
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackCAFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_A_SetB.Attack_A_SetB"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackCBFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_B_SetB.Attack_B_SetB"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackCCFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_C_SetB.Attack_C_SetB"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackCDFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_D_SetB.Attack_D_SetB"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AttackCEFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Attack_E_SetB.Attack_E_SetB"));

	if (AttackAAFinder.Succeeded()) AnimSeqArr.Add(AttackAAFinder.Object);
	if (AttackABFinder.Succeeded()) AnimSeqArr.Add(AttackABFinder.Object);
	if (AttackACFinder.Succeeded()) AnimSeqArr.Add(AttackACFinder.Object);
	if (AttackADFinder.Succeeded()) AnimSeqArr.Add(AttackADFinder.Object);

	if (AttackBAFinder.Succeeded()) AnimSeqArr.Add(AttackBAFinder.Object);
	if (AttackBBFinder.Succeeded()) AnimSeqArr.Add(AttackBBFinder.Object);
	if (AttackBCFinder.Succeeded()) AnimSeqArr.Add(AttackBCFinder.Object);
	if (AttackBDFinder.Succeeded()) AnimSeqArr.Add(AttackBDFinder.Object);
	if (AttackBEFinder.Succeeded()) AnimSeqArr.Add(AttackBEFinder.Object);

	if (AttackCAFinder.Succeeded()) AnimSeqArr.Add(AttackCAFinder.Object);
	if (AttackCBFinder.Succeeded()) AnimSeqArr.Add(AttackCBFinder.Object);
	if (AttackCCFinder.Succeeded()) AnimSeqArr.Add(AttackCCFinder.Object);
	if (AttackCDFinder.Succeeded()) AnimSeqArr.Add(AttackCDFinder.Object);
	if (AttackCEFinder.Succeeded()) AnimSeqArr.Add(AttackCEFinder.Object);

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

	int32 rng = FMath::Rand() % 3;
	switch (rng)
	{
	case(0):
		AttackType = ENEMYCHARACTER_COMBOATTACK_AA;
		break;
	case(1):
		AttackType = ENEMYCHARACTER_COMBOATTACK_BA;
		break;
	case(2):
		AttackType = ENEMYCHARACTER_COMBOATTACK_CA;
		break;
	default:
		AttackType = ENEMYCHARACTER_COMBOATTACK_AA;
		break;
	}

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
					DamageConfig.HitParticleType = PARTICLE_MINION_MELLEE_HIT_IMPACT;
					Damagable->RecieveDamage(DamageConfig);
				}
				//UE_LOG(LogTemp, Log, TEXT("ACMinion : PerformCapsuleTrace : %s"), *HitResult.GetActor()->GetName());
			}
		}
	}
}
