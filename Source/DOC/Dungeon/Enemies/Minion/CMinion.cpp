#include "CMinion.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Dungeon/Enemies/Minion/CAIController_Minion.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/IAnimInstance.h"
#include "GameSystem/CHitBoxComponent.h"
#include "Interfaces/IDamagable.h"
#include "Interfaces/CStageStructs.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Player/UI/CMonsterHP.h"

ACMinion::ACMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshMelleeFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Meshes/Minion_Lane_Melee_Core_Dawn.Minion_Lane_Melee_Core_Dawn"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshRangedFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Meshes/Minion_Lane_Ranged_Core_Dawn.Minion_Lane_Ranged_Core_Dawn"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshSiezeFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Meshes/Minion_Lane_Siege_Core_Dawn.Minion_Lane_Siege_Core_Dawn"));

	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPMelleeFinder(TEXT("/Game/Dungeon/Minion/AnimBP_Minion"));
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPRangedFinder(TEXT("/Game/Dungeon/Minion/AnimBP_Minion_Ranged"));
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPSiezeFinder(TEXT("/Game/Dungeon/Minion/AnimBP_Minion_Sieze"));

	if (SKMeshMelleeFinder.Succeeded()) SKMesh_Mellee	= SKMeshMelleeFinder.Object;
	if (SKMeshRangedFinder.Succeeded()) SKMesh_Ranged	= SKMeshRangedFinder.Object;
	if (SKMeshSiezeFinder.Succeeded()) SKMesh_Sieze		= SKMeshSiezeFinder.Object;

	if (AnimBPMelleeFinder.Succeeded()) AnimClass_Mellee	= AnimBPMelleeFinder.Class;
	if (AnimBPRangedFinder.Succeeded()) AnimClass_Ranged	= AnimBPRangedFinder.Class;
	if (AnimBPSiezeFinder.Succeeded()) AnimClass_Sieze		= AnimBPSiezeFinder.Class;

	// Animation
	AnimSeqArr.SetNum(ENEMYCHARACTER_NUM);
	AnimSeqArr[ENEMYCHARACTER_MINION].Reserve(ENEMYCHARACTER_COMBOATTACK_NUM);
	AnimSeqArr[ENEMYCHARACTER_MINION_RANGED].Reserve(ENEMYCHARACTER_RANGED_NUM);
	AnimSeqArr[ENEMYCHARACTER_MINION_SIEZE].Reserve(ENEMYCHARACTER_RANGED_NUM);

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

	ConstructorHelpers::FObjectFinder<UAnimSequence> AggroTransitionFinder	(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Ranged/Aggro_Transition_A.Aggro_Transition_A"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> FireAFinder			(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Ranged/Fire_A.Fire_A"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> TurnFastFinder			(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Ranged/TurnInPlace_Fast_Combat.TurnInPlace_Fast_Combat"));

	ConstructorHelpers::FObjectFinder<UAnimSequence> PlantFinder			(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Siege/PlantedIntro.PlantedIntro"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> PlantFireFinder		(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Siege/Fire_Planted.Fire_Planted"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> UnPlantFinder			(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Siege/UnPlanted.UnPlanted"));

	if (AttackAAFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackAAFinder.Object);
	if (AttackABFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackABFinder.Object);
	if (AttackACFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackACFinder.Object);
	if (AttackADFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackADFinder.Object);

	if (AttackBAFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackBAFinder.Object);
	if (AttackBBFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackBBFinder.Object);
	if (AttackBCFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackBCFinder.Object);
	if (AttackBDFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackBDFinder.Object);
	if (AttackBEFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackBEFinder.Object);

	if (AttackCAFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackCAFinder.Object);
	if (AttackCBFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackCBFinder.Object);
	if (AttackCCFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackCCFinder.Object);
	if (AttackCDFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackCDFinder.Object);
	if (AttackCEFinder.Succeeded()) AnimSeqArr[ENEMYCHARACTER_MINION].Add(AttackCEFinder.Object);

	if (AggroTransitionFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_MINION_RANGED].Add(AggroTransitionFinder.Object);
	if (FireAFinder.Succeeded())			AnimSeqArr[ENEMYCHARACTER_MINION_RANGED].Add(FireAFinder.Object);
	if (TurnFastFinder.Succeeded())			AnimSeqArr[ENEMYCHARACTER_MINION_RANGED].Add(TurnFastFinder.Object);

	if (PlantFinder.Succeeded())			AnimSeqArr[ENEMYCHARACTER_MINION_SIEZE].Add(PlantFinder.Object);
	if (PlantFireFinder.Succeeded())		AnimSeqArr[ENEMYCHARACTER_MINION_SIEZE].Add(PlantFireFinder.Object);
	if (UnPlantFinder.Succeeded())			AnimSeqArr[ENEMYCHARACTER_MINION_SIEZE].Add(UnPlantFinder.Object);

	HitBoxComponent = CreateDefaultSubobject<UCHitBoxComponent>(TEXT("HitBoxComponent"));

	// AI
	AIControllerClass = ACAIController_Minion::StaticClass();

	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTFinder(TEXT("/Game/Dungeon/Minion/BT_Minion.BT_Minion"));
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTRangedFinder(TEXT("/Game/Dungeon/Minion/BT_Minion_Ranged.BT_Minion_Ranged"));
	if (BTFinder.Succeeded()) BehaviorTree = BTFinder.Object;
	if (BTRangedFinder.Succeeded()) BehaviorTree_Ranged = BTRangedFinder.Object;
	MonsterHPComponent = CreateDefaultSubobject<UCMonsterHP>(TEXT("MonsterHPComponent"));
	//MonsterHPComponent->SetupAttachment(GetMesh());
	//MonsterHPComponent->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	MonsterHPComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
}

void ACMinion::BeginPlay()
{
	Super::BeginPlay();
	MonsterHPComponent->SetVisibility(false);
}

void ACMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter != nullptr)
	{
		//FRotator CamRot = PlayerCharacter->GetCameraTransform().GetRotation().Rotator();
		FVector CamLoc = PlayerCharacter->GetCameraTransform().GetLocation();
		FRotator tempRot = (GetActorLocation() - CamLoc).GetSafeNormal2D().Rotation();
		tempRot.Yaw += 180.f;
		tempRot.Normalize();
		MonsterHPComponent->SetTransform(
			GetActorLocation() + FVector(0.f, 0.f, 80.f),
			tempRot,
			DeltaTime
		);
		//MonsterHPComponent->SetWorldLocation(
		//	GetActorLocation() + FVector(0.f, 0.f, 180.f)
		//);
		//MonsterHPComponent->SetWorldRotation(
		//	(GetActorLocation() - CamLoc).GetSafeNormal2D().Rotation()
		//);
		//CamRot.Pitch *= 0.f;
		//CamRot.Yaw += 180.f;
		//CamRot.Roll *= 0.f;
		//MonsterHPComponent->SetRotation(CamRot.GetNormalized(), DeltaTime);
		//MonsterHPComponent->SetWorldRotation(CamRot.GetNormalized());
	}
}

void ACMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACMinion::SetEnemyType(int32 Type)
{
	EnemyType = Type;
	int32 rng = FMath::Rand() % 3;
	switch (EnemyType)
	{
	case(ENEMYCHARACTER_MINION):
		if (SKMesh_Mellee != nullptr) GetMesh()->SetSkeletalMesh(SKMesh_Mellee);
		if (AnimClass_Mellee != nullptr) GetMesh()->SetAnimClass(AnimClass_Mellee);
		if (GetCharacterMovement() != nullptr) GetCharacterMovement()->MaxWalkSpeed = 200.f;
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
		break;
	case(ENEMYCHARACTER_MINION_RANGED):
		if (SKMesh_Ranged != nullptr) GetMesh()->SetSkeletalMesh(SKMesh_Ranged);
		if (AnimClass_Ranged != nullptr) GetMesh()->SetAnimClass(AnimClass_Ranged);
		if (GetCharacterMovement() != nullptr) GetCharacterMovement()->MaxWalkSpeed = 150.f;
		break;
	default:
		break;
	}
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	ACAIController_Minion* AICon = GetWorld()->SpawnActor<ACAIController_Minion>(ACAIController_Minion::StaticClass());
	AnimInstance = Cast<IIAnimInstance>(GetMesh()->GetAnimInstance());
	AIController = Cast<IIEnemyAIController>(AICon);
	if (AICon != nullptr && AnimInstance != nullptr)
	{
		AICon->Possess(this);
		AnimInstance->OnPossess(this);
		AICon->SetupDelegates(AnimInstance->GetDelegate_MontagePlayingStateChanged());
	}
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
	if (AnimInstance != nullptr && AnimSeqArr[EnemyType].IsValidIndex(Type))
	{
		AnimInstance->PlayAnimation(AnimSeqArr[EnemyType][Type]);
	}
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
					DamageConfig.Causer = this;
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

FVector ACMinion::GetDealingCharacterLocation()
{
	AActor* temp = AIController->GetCurrentAttackTargetActor();
	return temp != nullptr ? temp->GetActorLocation() : FVector::ZeroVector;
}

void ACMinion::SetDealingCharacter(IIPlayerOnStage* DealingCharacter)
{
	PlayerCharacter = DealingCharacter;
	MonsterHPComponent->SetVisibility(PlayerCharacter != nullptr ? true : false);
}

void ACMinion::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	if (PlayerControllerStage != nullptr) PlayerControllerStage->LockOnMonster(this);
}

void ACMinion::SpawnProjectile(FTransform Transform)
{
	AActor* Target = AIController->GetCurrentAttackTargetActor();

	FDamageConfig DamageConfig;
	DamageConfig.Damage = 1.f;
	DamageConfig.Causer = this;
	DamageConfig.Instigator = GetController();
	DamageConfig.HitParticleType = PARTICLE_MINION_MELLEE_HIT_IMPACT;

	ObjectPoolManager->SpawnProjectile(GetActorTransform(), DamageConfig, Target, 1.f, PARTICLE_MINION_RANGED_PROJECTILE);
}

bool ACMinion::RecieveDamage(FDamageConfig DamageConfig)
{
	//IPCS->RecieveDamage(DamageConfig);
	LaunchCharacter(DamageConfig.HitDirection * DamageConfig.Damage * 500.f, true, false);
	DrawDebugDirectionalArrow(GetWorld(), DamageConfig.HitLocation - DamageConfig.Damage * 500.f, DamageConfig.HitLocation, 100.f, FColor::Red, false, 1.f, 0U, 1.f);
	return false;
}
