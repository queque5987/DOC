#include "CBoss.h"
#include "Player/UI/CMonsterHP.h"
#include "GameSystem/CStatComponent.h"
#include "GameSystem/CHitBoxComponent.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IAnimInstance.h"
#include "NiagaraComponent.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Dungeon/Enemies/Boss/CAIController_Boss.h"
#include "Components/SplineComponent.h"
#include "DrawDebugHelpers.h"
#include "CProjectile.h"
#include "GameFramework/GameStateBase.h"
#include "GameSystem/CNeuralNetwork.h"

ACBoss::ACBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshMelleeFinder(TEXT("/Game/Dungeon/Boss/Buff_Red/Meshes/Buff_Red.Buff_Red"));
	if (SKMeshMelleeFinder.Succeeded()) GetMesh()->SetSkeletalMesh(SKMeshMelleeFinder.Object);
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPMelleeFinder(TEXT("/Game/Dungeon/Boss/AnimBP_Boss"));
	if (AnimBPMelleeFinder.Succeeded()) AnimClass_Boss = AnimBPMelleeFinder.Class;

	HitBoxComponent = CreateDefaultSubobject<UCHitBoxComponent>(TEXT("HitBoxComponent"));
	MonsterHPComponent = CreateDefaultSubobject<UCMonsterHP>(TEXT("MonsterHPComponent"));
	MonsterHPComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	StatComponent = CreateDefaultSubobject<UCStatComponent>(TEXT("StatComponent"));

	OnDeathNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("OnDeathNiagaraComponent"));
	OnDeathNiagaraComponent->SetupAttachment(GetRootComponent());
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemFinder(TEXT("/Game/Dungeon/FX/FX_MinionDeath.FX_MinionDeath"));
	if (NiagaraSystemFinder.Succeeded())
	{
		OnDeathNiagaraComponent->SetAsset(NiagaraSystemFinder.Object);
		OnDeathNiagaraComponent->SetAutoActivate(false);
	}

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	//SplineComponent->SetupAttachment(GetRootComponent());

	SplineComponent->ClearSplinePoints(true);
	SplineComponent->AddSplinePoint(FVector(0.f, 0.f, 0.f), ESplineCoordinateSpace::Local);
	SplineComponent->AddSplinePoint(FVector(500.f, 0.f, 0.f), ESplineCoordinateSpace::Local);
	SplineComponent->AddSplinePoint(FVector(1000.f, 0.f, 0.f), ESplineCoordinateSpace::Local);

	AnimSeqArr.SetNum(ENEMYCHARACTER_ACTIONTYPE_NUM);
	ConstructorHelpers::FObjectFinder<UAnimSequence> MelleAttackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_Punch_01.Attack_Punch_01"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> RangedAttackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_Uppercut.Attack_Uppercut"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> ChargeFinder		(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Knock_Bwd.Knock_Bwd"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> JumpChargeFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_BigSmash.Attack_BigSmash"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> ComboAttackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_SmallCombo.Attack_SmallCombo"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> HeavyAttackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_Punch_02.Attack_Punch_02"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AlignAxis_LFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Turn_Left_90.Turn_Left_90"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> AlignAxis_RFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Turn_Right_90.Turn_Right_90"));

	if (MelleAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK] = MelleAttackFinder.Object;
	if (RangedAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK] = RangedAttackFinder.Object;
	if (ChargeFinder.Succeeded())		AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_CHARGE] = ChargeFinder.Object;
	if (JumpChargeFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE] = JumpChargeFinder.Object;
	if (ComboAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK] = ComboAttackFinder.Object;
	if (HeavyAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK] = HeavyAttackFinder.Object;
	if (AlignAxis_LFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_L] = AlignAxis_LFinder.Object;
	if (AlignAxis_RFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_R] = AlignAxis_RFinder.Object;

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimClass(AnimClass_Boss);

	ConstructorHelpers::FObjectFinder<UParticleSystem> ProjectileFinder (TEXT("/Game/Dungeon/Minion/Particles/Minions/Dragon/FX/P_Dragon_Fireball_Projectile_s.P_Dragon_Fireball_Projectile_s"));
	if (ProjectileFinder.Succeeded()) ProjectileParticle = ProjectileFinder.Object;

	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTFinder(TEXT("/Game/Dungeon/Boss/BT_Boss.BT_Boss"));
	if (BTFinder.Succeeded()) BehaviorTree = BTFinder.Object;

	AIControllerClass = ACAIController_Boss::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACBoss::BeginPlay()
{
	Super::BeginPlay();
	MonsterHPComponent->SetVisibility(false);

	if (StatComponent && MonsterHPComponent)
	{
		MonsterHPComponent->SetDelegates(&StatComponent->OnStatusChanged, nullptr, nullptr);
		StatComponent->OnDeath.AddUFunction(this, TEXT("Died"));
		StatComponent->SetupDelegates(&OnReceivedDamageDelegate, nullptr);
	}
	AnimInstance = Cast<IIAnimInstance>(GetMesh()->GetAnimInstance());
	OnEnemyActionDelegate.AddUFunction(this, TEXT("OnEnemyAction"));

	AIController = Cast<IIEnemyAIController>(GetController());
	if (SplineComponent) SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	if (GetWorld()) ObjectPoolManager = Cast<IIObjectPoolManager>(GetWorld()->GetGameState());
	//if (NeuralNetworkModel != nullptr) NeuralNetworkModel->LoadModelFromPath();
	UCNeuralNetwork* NN = NewObject<UCNeuralNetwork>(this);
	NN->LoadModelFromPath();
	//NN->URunModel();
}

void ACBoss::Select()
{
	if (Selected) return;
	Selected = true;
	if (GetMesh() != nullptr)
	{
		GetMesh()->SetRenderCustomDepth(true);
		GetMesh()->SetCustomDepthStencilValue(CUSTOMDEPTH_ENEMYCHARACTER);
	}
}

void ACBoss::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (GetMesh() != nullptr)
	{
		GetMesh()->SetRenderCustomDepth(false);
	}
}

void ACBoss::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	if (PlayerControllerStage != nullptr) PlayerControllerStage->LockOnMonster(this);
}

void ACBoss::SetEnabled(bool e)
{
	GetMesh()->SetVisibility(e);
	if (e)
	{
		if (StatComponent != nullptr) StatComponent->SetCurrentHP(StatComponent->GetMaxHP());
	}
}

bool ACBoss::GetBusy()
{
	return (AnimInstance != nullptr ? AnimInstance->GetBusy() : false);
}

FVector ACBoss::GetDealingCharacterLocation()
{
	return AIController != nullptr && AIController->GetCurrentAttackTargetActor() != nullptr ? AIController->GetCurrentAttackTargetActor()->GetActorLocation() : GetActorLocation();
}

void ACBoss::SpawnProjectile(FTransform Transform, FDamageConfig DamageConfig)
{
	if (SplineComponent == nullptr || ObjectPoolManager == nullptr) return;
	DamageConfig.Causer = this;
	DamageConfig.Instigator = GetController();
	DamageConfig.AttackType = ATTACK_TYPE_RANGED;
	DamageConfig.DamageWidgetColor = DAMAGE_COLOR_MINION;
	DamageConfig.CausedTimeSeconds = GetWorld()->TimeSeconds;
	AActor* Target = AIController ? AIController->GetCurrentAttackTargetActor() : nullptr;
	SplineComponent->SetLocationAtSplinePoint(0, Transform.GetLocation(), ESplineCoordinateSpace::World);
	SplineComponent->UpdateSpline();
	//ObjectPoolManager->SpawnProjectile(Transform, DamageConfig, Target, 750.f, ProjectileParticle, SplineComponent);

	FRotator ThrowRot = FRotator(40.f, 56.f, 23.f);

	ObjectPoolManager->SpawnProjectile(Transform, DamageConfig, Target, 750.f, ThrowRot, ProjectileParticle);
}

FTransform ACBoss::GetSplineTransformAtTime(float Time)
{
	if (SplineComponent)
	{
		return SplineComponent->GetTransformAtTime(Time, ESplineCoordinateSpace::World, true);
	}

	return GetActorTransform();
}

void ACBoss::LaunchCharacter_Direction(FVector Direction, float Force)
{
	LaunchCharacter(Direction * Force, false, false);
}

void ACBoss::ResetTraceProperties()
{
	if (HitBoxComponent != nullptr)
	{
		HitBoxComponent->ResetProperties();
		HitBoxComponent->SetDebug(true);
	}
}

bool ACBoss::PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, FDamageConfig DamageConfig)
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

void ACBoss::Died(FDamageConfig DamageConfig)
{
}

void ACBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SplineComponent)
	{
		const int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
		for (int32 i = 0; i < NumPoints; ++i)
		{
			const FVector PointLocation = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			DrawDebugSphere(GetWorld(), PointLocation, 20.f, 12, FColor::Red, false, -1.f, 0, 2.f);
		}

		const int32 NumSegments = 30;
		for (int32 i = 0; i < NumSegments; ++i)
		{
			const FVector StartPoint = SplineComponent->GetLocationAtTime(i / NumSegments, ESplineCoordinateSpace::World);
			const FVector EndPoint = SplineComponent->GetLocationAtTime((i + 1) / NumSegments, ESplineCoordinateSpace::World);
			DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, -1.f, 0, 2.f);
		}
	}
}

void ACBoss::OnEnemyAction(int32 ActionType)
{
	UE_LOG(LogTemp, Log, TEXT("ACBoss : OnEnemyAction ActionType : %d"), ActionType);
	if (!AnimSeqArr.IsValidIndex(ActionType) || AnimInstance == nullptr) return;
	AnimInstance->PlayAnimation(AnimSeqArr[ActionType]);

	if (AIController && AIController->GetCurrentAttackTargetActor())
	{
		SplineComponent->SetRelativeRotation(GetActorRotation());
		FVector TargetLocation = AIController->GetCurrentAttackTargetActor()->GetActorLocation();
		FVector CurrLocation = GetActorLocation();
		float TCDist = FVector::Dist2D(TargetLocation, CurrLocation);
		FVector MidLocation = (TargetLocation + (TargetLocation + CurrLocation) / 2.f) / 2.f;
		FVector VerticalTangent = FVector::ZeroVector;
		FVector TCDirection = (CurrLocation - TargetLocation).GetSafeNormal2D();

		switch (ActionType)
		{
		case(ENEMYCHARACTER_ACTIONTYPE_CHARGE):
			SplineComponent->SetLocationAtSplinePoint(0, CurrLocation, ESplineCoordinateSpace::World, false);
			SplineComponent->SetLocationAtSplinePoint(1, CurrLocation + TCDirection * TCDist * 0.625f, ESplineCoordinateSpace::World, false);
			SplineComponent->SetLocationAtSplinePoint(2, CurrLocation + TCDirection * TCDist * 1.25f, ESplineCoordinateSpace::World, false);
			for (int32 i = 0; i < 3; ++i)
			{
				SplineComponent->SetSplinePointType(i, ESplinePointType::Constant, false);
			}
			SplineComponent->UpdateSpline();
			break;
		case(ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK):
			//MidLocation += -GetActorRightVector() * (75.f + FMath::Clamp(TCDist / 500.f, 0.f, 1.f) * 75.f);
			//MidLocation.Z = TargetLocation.Z;// -FMath::Clamp(TCDist / 500.f, 0.f, 1.f) * 75.f;
			//SplineComponent->SetLocationAtSplinePoint(0, CurrLocation, ESplineCoordinateSpace::World, false);
			//SplineComponent->SetLocationAtSplinePoint(1, MidLocation, ESplineCoordinateSpace::World, false);
			//SplineComponent->SetLocationAtSplinePoint(2, TargetLocation, ESplineCoordinateSpace::World, false);
			//for (int32 i = 0; i < 3; ++i)
			//{
			//	SplineComponent->SetSplinePointType(i, ESplinePointType::CurveClamped, false);
			//}
			//SplineComponent->SetTangentAtSplinePoint(0, FVector(300.f, -1500.f, -150.f), ESplineCoordinateSpace::Local, false);
			//SplineComponent->SetTangentAtSplinePoint(1, FVector(250.f, 150.f, 150.f), ESplineCoordinateSpace::Local, false);
			//SplineComponent->SetTangentAtSplinePoint(2, FVector(350.f, -350.f, 0.f), ESplineCoordinateSpace::Local, false);
			//SplineComponent->UpdateSpline();
			break;
		case(ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE):
			MidLocation.Z = TargetLocation.Z + 250.f + (TCDist / 1500.f) * 250.f;
			TargetLocation += TCDirection * 150.f;
			SplineComponent->SetLocationAtSplinePoint(0, CurrLocation, ESplineCoordinateSpace::World, false);
			SplineComponent->SetLocationAtSplinePoint(1, MidLocation, ESplineCoordinateSpace::World, false);
			SplineComponent->SetLocationAtSplinePoint(2, TargetLocation, ESplineCoordinateSpace::World, false);
			for (int32 i = 0; i < 3; ++i)
			{
				SplineComponent->SetSplinePointType(i, ESplinePointType::CurveClamped, false);
			}
			VerticalTangent = FVector(0, 0, MidLocation.Z);
			SplineComponent->SetTangentAtSplinePoint(0, VerticalTangent, ESplineCoordinateSpace::Local, false);
			SplineComponent->SetTangentAtSplinePoint(1, FVector(TCDist / 2.f, 0.f, 0.f), ESplineCoordinateSpace::Local, false);
			SplineComponent->SetTangentAtSplinePoint(2, -VerticalTangent / 2.f, ESplineCoordinateSpace::Local, false);
			SplineComponent->UpdateSpline();
			break;
		default:
			break;
		}
	}
}

