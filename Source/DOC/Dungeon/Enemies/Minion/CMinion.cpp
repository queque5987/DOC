#include "CMinion.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Dungeon/Enemies/Minion/CAIController_Minion.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameSystem/CHitBoxComponent.h"
#include "Interfaces/IAnimInstance.h"
#include "Interfaces/IDamagable.h"
#include "Interfaces/CStageStructs.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Player/UI/CMonsterHP.h"
#include "GameSystem/CStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/CHttpComponent.h"

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
	AnimSeqArr_HitReact.SetNum(4);
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

	ConstructorHelpers::FObjectFinder<UAnimSequence> DeathFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Death_A.Death_A"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> GroggyFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/Stun.Stun"));

	ConstructorHelpers::FObjectFinder<UAnimSequence> AggroTransitionFinder	(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Ranged/Aggro_Transition_A.Aggro_Transition_A"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> FireAFinder			(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Ranged/Fire_A.Fire_A"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> TurnFastFinder			(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Ranged/TurnInPlace_Fast_Combat.TurnInPlace_Fast_Combat"));

	ConstructorHelpers::FObjectFinder<UAnimSequence> PlantFinder			(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Siege/PlantedIntro.PlantedIntro"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> PlantFireFinder		(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Siege/Fire_Planted.Fire_Planted"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> UnPlantFinder			(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Siege/UnPlanted.UnPlanted"));

	ConstructorHelpers::FObjectFinder<UAnimSequence> HitReact_FrontFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/HitReact_Front.HitReact_Front"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> HitReact_BackFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/HitReact_Back.HitReact_Back"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> HitReact_LeftFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/HitReact_Left.HitReact_Left"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> HitReact_RightFinder(TEXT("/Game/Dungeon/Minion/Down_Minions/Animations/Melee/HitReact_Right.HitReact_Right"));

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

	if (HitReact_FrontFinder.Succeeded()) AnimSeqArr_HitReact[ENEMYCHARACTER_HIT_REACT_FRONT] = (HitReact_FrontFinder.Object);
	if (HitReact_BackFinder.Succeeded()) AnimSeqArr_HitReact[ENEMYCHARACTER_HIT_REACT_BACK] = (HitReact_BackFinder.Object);
	if (HitReact_LeftFinder.Succeeded()) AnimSeqArr_HitReact[ENEMYCHARACTER_HIT_REACT_LEFT] = (HitReact_LeftFinder.Object);
	if (HitReact_RightFinder.Succeeded()) AnimSeqArr_HitReact[ENEMYCHARACTER_HIT_REACT_RIGHT] = (HitReact_RightFinder.Object);

	if (DeathFinder.Succeeded())	DeathAnimSeq = DeathFinder.Object;
	if (GroggyFinder.Succeeded())	GroggyhAnimSeq = GroggyFinder.Object;
	
	if (AggroTransitionFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_MINION_RANGED].Add(AggroTransitionFinder.Object);
	if (FireAFinder.Succeeded())			AnimSeqArr[ENEMYCHARACTER_MINION_RANGED].Add(FireAFinder.Object);
	if (TurnFastFinder.Succeeded())			AnimSeqArr[ENEMYCHARACTER_MINION_RANGED].Add(TurnFastFinder.Object);

	if (PlantFinder.Succeeded())			AnimSeqArr[ENEMYCHARACTER_MINION_SIEZE].Add(PlantFinder.Object);
	if (PlantFireFinder.Succeeded())		AnimSeqArr[ENEMYCHARACTER_MINION_SIEZE].Add(PlantFireFinder.Object);
	if (UnPlantFinder.Succeeded())			AnimSeqArr[ENEMYCHARACTER_MINION_SIEZE].Add(UnPlantFinder.Object);

	HitBoxComponent = CreateDefaultSubobject<UCHitBoxComponent>(TEXT("HitBoxComponent"));

	// AI
	//AIControllerClass = ACAIController_Minion::StaticClass();
	AutoPossessAI = EAutoPossessAI::Disabled;

	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTFinder(TEXT("/Game/Dungeon/Minion/BT_Minion.BT_Minion"));
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTRangedFinder(TEXT("/Game/Dungeon/Minion/BT_Minion_Ranged.BT_Minion_Ranged"));
	if (BTFinder.Succeeded()) BehaviorTree = BTFinder.Object;
	if (BTRangedFinder.Succeeded()) BehaviorTree_Ranged = BTRangedFinder.Object;
	MonsterHPComponent = CreateDefaultSubobject<UCMonsterHP>(TEXT("MonsterHPComponent"));
	MonsterHPComponent->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));

	StatComponent = CreateDefaultSubobject<UCStatComponent>(TEXT("StatComponent"));

	OnDeathNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("OnDeathNiagaraComponent"));
	OnDeathNiagaraComponent->SetupAttachment(GetRootComponent());

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemFinder(TEXT("/Game/Dungeon/FX/FX_MinionDeath.FX_MinionDeath"));
	if (NiagaraSystemFinder.Succeeded())
	{
		OnDeathNiagaraComponent->SetAsset(NiagaraSystemFinder.Object);
		OnDeathNiagaraComponent->SetAutoActivate(false);
	}

	HttpComponent = CreateDefaultSubobject<UCHttpComponent>(TEXT("HttpComponent"));
}

void ACMinion::BeginPlay()
{
	Super::BeginPlay();
	MonsterHPComponent->SetVisibility(false);

	if (StatComponent && MonsterHPComponent)
	{
		MonsterHPComponent->SetDelegates(&StatComponent->OnStatusChanged, GetOnGroggyDelegate(), &OnGroggyEndDelegate);
		StatComponent->OnDeath.AddUFunction(this, TEXT("Died"));
		StatComponent->OnGroggy.AddUFunction(this, TEXT("Groggy"));
		StatComponent->SetupDelegates(&OnReceivedDamageDelegate, &OnGroggyEndDelegate);
	}
}

void ACMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MonsterHPComponent != nullptr && PlayerCharacter != nullptr)
	{
		const FVector CamLoc = PlayerCharacter->GetCameraTransform().GetLocation();
		FRotator tempRot = (GetActorLocation() - CamLoc).GetSafeNormal().Rotation();
		tempRot.Yaw += 180.f;
		tempRot.Pitch *= -1.f;
		tempRot.Normalize();
		MonsterHPComponent->SetTransform(
			GetActorLocation() + FVector(0.f, 0.f, 80.f),
			tempRot,
			DeltaTime
		);
	}

	if (HttpComponent != nullptr && GetWorld() && PlayerCharacter != nullptr)
	{
		float DistFromTop = 0.f;
		float DistFromBottom = 0.f;
		float DistFromLeft = 0.f;
		float DistFromRight = 0.f;
		PlayerCharacter->GetRoomRelativeLocation(DistFromTop, DistFromBottom, DistFromLeft, DistFromRight);
		FPlayerStat* PlayerStat = PlayerCharacter->GetCurrentPlayerStatus();
		if (PlayerStat != nullptr && (DistFromTop + DistFromBottom + DistFromLeft + DistFromRight) > 0.f)
		{
			HttpComponent->AddTimeSeriesData(
				PlayerCharacter->GetCurrentPressingButton(),
				FVector::Dist2D(PlayerCharacter->GetLocation(), GetLocation()),
				DistFromTop,
				DistFromBottom,
				DistFromLeft,
				DistFromRight,
				PlayerStat->CurrHP,
				PlayerStat->CurrMP
			);
		}
	}
}

void ACMinion::SetEnabled(bool e)
{
	GetMesh()->SetVisibility(e);
	if (e)
	{
		if (StatComponent != nullptr) StatComponent->SetCurrentHP(StatComponent->GetMaxHP());
	}
}

void ACMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACMinion::SetEnemyType(int32 Type)
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Dying = false;
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
	//ACAIController_Minion* AICon = Cast<ACAIController_Minion>(GetController());
	AnimInstance = Cast<IIAnimInstance>(GetMesh()->GetAnimInstance());
	AIController = Cast<IIEnemyAIController>(AICon);
	if (AICon != nullptr && AnimInstance != nullptr)
	{
		AICon->Possess(this);
		AnimInstance->OnPossess(this);
		AnimInstance->SetupDelegates(nullptr, &OnReceivedDamageDelegate, GetOnGroggyDelegate(), &OnGroggyEndDelegate);
		AICon->SetupDelegates(AnimInstance->GetDelegate_MontagePlayingStateChanged(), &OnReceivedDamageDelegate);
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
			if (HitResult.GetActor() != nullptr && !HitResult.GetActor()->Implements<UIEnemyCharacter>())
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
					DamageConfig.AttackType = ATTACK_TYPE_MELLE;
					DamageConfig.DamageWidgetColor = DAMAGE_COLOR_MINION;
					DamageConfig.CausedTimeSeconds = GetWorld()->TimeSeconds;
					Damagable->RecieveDamage(DamageConfig);
				}
			}
		}
	}
}

bool ACMinion::PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, FDamageConfig DamageConfig)
{
	bool rtn = false;
	if (HitBoxComponent != nullptr)
	{
		FVector SwingDirection;
		TArray<FHitResult> temp = HitBoxComponent->PerformCapsuleTrace<UIDamagable>(CapsuleRadius, CapsuleHalfHeight, Location, Rotation, Precision, SwingDirection);

		for (FHitResult HitResult : temp)
		{
			if (HitResult.GetActor() != nullptr && !HitResult.GetActor()->Implements<UIEnemyCharacter>())
			{
				IIDamagable* Damagable = Cast<IIDamagable>(HitResult.GetActor());
				if (Damagable != nullptr)
				{
					DamageConfig.Causer = this;
					DamageConfig.Instigator = GetController();
					DamageConfig.HitDirection = SwingDirection;
					DamageConfig.HitLocation = HitResult.ImpactPoint;
					DamageConfig.HitParticleType = PARTICLE_MINION_MELLEE_HIT_IMPACT;
					DamageConfig.AttackType = ATTACK_TYPE_MELLE;
					DamageConfig.DamageWidgetColor = DAMAGE_COLOR_MINION;
					DamageConfig.CausedTimeSeconds = GetWorld()->TimeSeconds;
					if (!rtn) rtn = Damagable->RecieveDamage(DamageConfig);
				}
			}
		}
	}
	return rtn;
}

FVector ACMinion::GetDealingCharacterLocation()
{
	AActor* temp = AIController->GetCurrentAttackTargetActor();
	return temp != nullptr ? temp->GetActorLocation() : FVector::ZeroVector;
}

void ACMinion::SetDealingCharacter(IIPlayerOnStage* DealingCharacter)
{
	PlayerCharacter = DealingCharacter;
	MonsterHPComponent->SetVisibility((StatComponent && StatComponent->GetCurrentHP() > 0.f && PlayerCharacter != nullptr) ? true : false);
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
	DamageConfig.HitEffect = nullptr;
	DamageConfig.HitSound = nullptr;
	DamageConfig.AttackType = ATTACK_TYPE_RANGED;

	ObjectPoolManager->SpawnProjectile(GetActorTransform(), DamageConfig, Target, 1.f, PARTICLE_MINION_RANGED_PROJECTILE);
}

void ACMinion::SpawnProjectile(FTransform Transform, FDamageConfig DamageConfig)
{
	if (ObjectPoolManager == nullptr) return;
	AActor* Target = AIController->GetCurrentAttackTargetActor();
	DamageConfig.Causer = this;
	DamageConfig.Instigator = GetController();
	DamageConfig.AttackType = ATTACK_TYPE_RANGED;

	ObjectPoolManager->SpawnProjectile(Transform, DamageConfig, Target, 750.f, DamageConfig.Projectile);
}

bool ACMinion::RecieveDamage(FDamageConfig DamageConfig)
{
	if (Dying) return false;
	OnReceivedDamageDelegate.Broadcast(DamageConfig);
	LaunchCharacter(DamageConfig.HitDirection * DamageConfig.Damage * 20.f, true, false);
	return true;
}

FOnDeath* ACMinion::GetOnDeathDelegate()
{
	return StatComponent != nullptr ? &StatComponent->OnDeath : nullptr;
}

FOnDeath* ACMinion::GetOnDiedCompletedDelegate()
{
	return &MinionDiedCompletedDelegate;
}

FOnGroggy* ACMinion::GetOnGroggyDelegate()
{
	return StatComponent != nullptr ? &StatComponent->OnGroggy : nullptr;
}

FOnGroggyEnd* ACMinion::GetOnGroggyEndDelegate()
{
	return &OnGroggyEndDelegate;
}

void ACMinion::PlayDiedFX(int32 FXSequence)
{
	if (FXSequence == 0)
	{
		if (OnDeathNiagaraComponent != nullptr)
		{
			OnDeathNiagaraComponent->Activate();
			OnDeathNiagaraComponent->ActivateSystem();
		}
	}
	else if (FXSequence == 1)
	{
		ObjectPoolManager->SpawnParticle(nullptr, NAME_None, PARTICLE_MINION_DEAD_RECALL, GetActorTransform());
	}
	else if (FXSequence == 2)
	{
		GetMesh()->SetVisibility(false);
		FDamageConfig tempDamConfig;
		tempDamConfig.Causer = this;
		MinionDiedCompletedDelegate.Broadcast(tempDamConfig);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (HttpComponent != nullptr) HttpComponent->SendRequest();
	}
}

class UAnimSequence* ACMinion::GetHitReactAnimSequence(int32 HitDirection)
{
	if (AnimSeqArr_HitReact.IsValidIndex(HitDirection))
	{
		return AnimSeqArr_HitReact[HitDirection];
	}
	return nullptr;
}

void ACMinion::Died(FDamageConfig DamageConfig)
{
	if (AnimInstance != nullptr && DeathAnimSeq != nullptr)
	{
		Dying = true;
		AnimInstance->PlayAnimation(DeathAnimSeq);
	}
	UE_LOG(LogTemp, Log, TEXT("This Minion Deseased"));
}

void ACMinion::Groggy(FDamageConfig DamageConfig)
{
	UE_LOG(LogTemp, Log, TEXT("This Minion Is On Groggy"));
	GetWorld()->GetTimerManager().ClearTimer(GroggyTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(GroggyTimerHandle, FTimerDelegate::CreateLambda([&] {
		OnGroggyEndDelegate.Broadcast();
		}), 2.f, false);
}

void ACMinion::Execute(FDamageConfig DamageConfig)
{
	if (DamageConfig.Causer == nullptr) return;
	GetWorld()->GetTimerManager().ClearTimer(GroggyTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(GroggyTimerHandle, FTimerDelegate::CreateLambda([&] {
		OnGroggyEndDelegate.Broadcast();
		}), 1.63f, false);
	OnBeExecuted.Broadcast(DamageConfig);
	FVector Direction = (DamageConfig.Causer->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	SetActorRotation(Direction.Rotation());
}

bool ACMinion::IsExecutable()
{
	return StatComponent != nullptr ? StatComponent->IsGroggy() : false;
}

void ACMinion::Stun(float Duration, FDamageConfig DamageConfig)
{
	if (GetOnGroggyDelegate() != nullptr)
	{
		AIController->Groggy(FDamageConfig());
		GetWorld()->GetTimerManager().ClearTimer(GroggyTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(GroggyTimerHandle, FTimerDelegate::CreateLambda([&] {
			AIController->GroggyEnd();
			}), Duration, false);
	}
}
