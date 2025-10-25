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
#include "Kismet/GameplayStatics.h"
#include "GameSystem/CNeuralNetwork.h"

ACBoss::ACBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshMelleeFinder(TEXT("/Game/Dungeon/Boss/Buff_Red/Meshes/Buff_Red.Buff_Red"));
	if (SKMeshMelleeFinder.Succeeded()) GetMesh()->SetSkeletalMesh(SKMeshMelleeFinder.Object);
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPMelleeFinder(TEXT("/Game/Dungeon/Boss/AnimBP_Boss"));
	if (AnimBPMelleeFinder.Succeeded()) AnimClass_Boss = AnimBPMelleeFinder.Class;

	HitBoxComponent = CreateDefaultSubobject<UCHitBoxComponent>(TEXT("HitBoxComponent"));
	//MonsterHPComponent = CreateDefaultSubobject<UCMonsterHP>(TEXT("MonsterHPComponent"));
	//MonsterHPComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
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
	ConstructorHelpers::FObjectFinder<UAnimSequence> ExecutedFinder		(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Knock_Fwd.Knock_Fwd"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> DeathBackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Death_Back.Death_Back"));
	
	if (MelleAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK] = MelleAttackFinder.Object;
	if (RangedAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK] = RangedAttackFinder.Object;
	if (ChargeFinder.Succeeded())		AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_CHARGE] = ChargeFinder.Object;
	if (JumpChargeFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE] = JumpChargeFinder.Object;
	if (RangedAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_UPPERCUT] = RangedAttackFinder.Object;
	if (ComboAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK] = ComboAttackFinder.Object;
	if (HeavyAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK] = HeavyAttackFinder.Object;
	if (AlignAxis_LFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_L] = AlignAxis_LFinder.Object;
	if (AlignAxis_RFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_ALIGN_AXIS_R] = AlignAxis_RFinder.Object;

	if (ExecutedFinder.Succeeded())		GroggyhAnimSeq = ExecutedFinder.Object;
	if (DeathBackFinder.Succeeded())	DeathAnimSeq = DeathBackFinder.Object;

	GetMesh()->SetRelativeLocation(PrimeRelativeLocation);
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
	//MonsterHPComponent->SetVisibility(false);

	if (StatComponent)
	{
		//MonsterHPComponent->SetDelegates(&StatComponent->OnStatusChanged, nullptr, nullptr);
		StatComponent->OnDeath.AddUFunction(this, TEXT("Died"));
		StatComponent->OnGroggy.AddUFunction(this, TEXT("Groggy"));
		StatComponent->SetupDelegates(&OnReceivedDamageDelegate, &OnGroggyEndDelegate);
		OnGroggyDelegatePtr = &StatComponent->OnGroggy;
	}
	AnimInstance = Cast<IIAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnPossess(this);
	OnEnemyActionDelegate.AddUFunction(this, TEXT("OnEnemyAction"));

	AIController = Cast<IIEnemyAIController>(GetController());
	if (SplineComponent) SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	if (GetWorld()) ObjectPoolManager = Cast<IIObjectPoolManager>(GetWorld()->GetGameState());
	//if (NeuralNetworkModel != nullptr) NeuralNetworkModel->LoadModelFromPath();
	NN = NewObject<UCNeuralNetwork>(this);
	NN->InitializeModel();
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
		if (StatComponent != nullptr)
		{
			StatComponent->SetMaxHP(1000.f);
			StatComponent->SetCurrentHP(StatComponent->GetMaxHP());
			StatComponent->SetMaxGroggy(500.f);
			StatComponent->SetGroggy(0.f);
		}
		AnimInstance = Cast<IIAnimInstance>(GetMesh()->GetAnimInstance());
		AnimInstance->SetupDelegates(nullptr, nullptr, GetOnGroggyDelegate(), GetOnGroggyEndDelegate());
		GetMesh()->bPauseAnims = false;
	}
	Dying_Submerge = false;
}

bool ACBoss::GetBusy()
{
	return (AnimInstance != nullptr ? AnimInstance->GetBusy() || AnimInstance->GetGroggy() : false);
}

FVector ACBoss::GetDealingCharacterLocation()
{
	return AIController != nullptr && AIController->GetCurrentAttackTargetActor() != nullptr ? AIController->GetCurrentAttackTargetActor()->GetActorLocation() : GetActorLocation();
}

void ACBoss::ManualMoveToDirection(FVector Direction)
{
	AddMovementInput(Direction);
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

FOnDeath* ACBoss::GetOnDeathDelegate()
{
	return StatComponent != nullptr ? &StatComponent->OnDeath : nullptr;
}

FOnGroggy* ACBoss::GetOnGroggyDelegate()
{
	return StatComponent != nullptr ? &StatComponent->OnGroggy : nullptr;
}

void ACBoss::PlayDiedFX(int32 FXSequence, UParticleSystem* PlayParticle, FTransform SpawnAdjustTransform)
{
	if (PlayParticle != nullptr)
	{
		FRotator SpawnRotation = GetActorRotation() + SpawnAdjustTransform.GetRotation().Rotator();
		FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(SpawnAdjustTransform.GetLocation());
		FVector SpawnScale = SpawnAdjustTransform.GetScale3D();
		FTransform SpawnTransform{ SpawnRotation, SpawnLocation, SpawnScale };
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayParticle, SpawnTransform, true, EPSCPoolMethod::AutoRelease);
	}
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
		Dying_Submerge = true;
	}
	else if (FXSequence == 2)
	{
		GetMesh()->bPauseAnims = true;
		FTimerManager& TM = GetWorld()->GetTimerManager();
		TM.ClearTimer(DeadPauseAnimTimerHandle);
		TM.SetTimer(DeadPauseAnimTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				GetMesh()->bPauseAnims = false;
			}), 3.f, false
		);
	}
	else if (FXSequence == 3)
	{
		FDamageConfig tempDamConfig;
		tempDamConfig.Causer = this;
		Dying_Submerge = false;
		GetMesh()->SetVisibility(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetRelativeLocation(PrimeRelativeLocation);
		MinionDiedCompletedDelegate.Broadcast(tempDamConfig);
	}
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

float ACBoss::InferencePlayerNextMove(TArray<float> InputData)
{
	// Test
	//TArray<float> randomarrays;
	//randomarrays.SetNum(35);
	//for (float& ra : randomarrays)
	//{
	//	ra = FMath::FRandRange(0.f, 100.f);
	//}
	//NN->RunInference(randomarrays);
	float Output = 0.f;
	//NN->RunInference(InputData, Output);
	return Output;
}

float ACBoss::InferencePlayerNextMove(IIPlayerOnStage* InPlayerCharacter)
{
	//FPlayerTimeSeriesDataV2 CurrTimeSerieseData;
	TArray<FPlayerTimeSeriesDataV3> CurrTimeSerieseDataArr;
	float InferencedMove = -1.f;
	//InPlayerCharacter->CreateTimeSeriesData(GetActorLocation(), CurrTimeSerieseData);
	InPlayerCharacter->CreateTimeSeriesData(GetActorLocation(), CurrTimeSerieseDataArr);
	if (CurrTimeSerieseDataArr.Num() > 9)
	{
		//NN->RunInference(CurrTimeSerieseData, InferencedMove);
		NN->RunInference(CurrTimeSerieseDataArr, InferencedMove);
		FVector InferencedDirection{
			FMath::Cos(InferencedMove),
			FMath::Sin(InferencedMove),
			0.f
		};
		InferencedDirection.Normalize();
		FVector AbsoluteDirection = InPlayerCharacter->GetRotation().RotateVector(InferencedDirection);
		DrawDebugDirectionalArrow(GetWorld(), InPlayerCharacter->GetLocation(), InPlayerCharacter->GetLocation() + AbsoluteDirection * 300.f, 150.f, FColor::Cyan, false, 2.f, false, 20.f);
		UE_LOG(LogTemp, Log, TEXT("ACBoss::InferencePlayerNextMove : Inferenced Direction : %s"), *InferencedDirection.ToString());
	}
	return InferencedMove;
}

void ACBoss::Groggy(FDamageConfig DamageConfig)
{
	if (GetWorld() == nullptr) return;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(GroggyTimerHandle);
	TimerManager.SetTimer(GroggyTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			OnGroggyEndDelegate.Broadcast();
		}
	), 5.f, false);	
}

bool ACBoss::RecieveDamage(FDamageConfig DamageConfig)
{
	if (Dying) return false;
	OnReceivedDamageDelegate.Broadcast(DamageConfig);
	//LaunchCharacter(DamageConfig.HitDirection * DamageConfig.Damage * 20.f, true, false);
	return true;
}

void ACBoss::ResetTraceProperties()
{
	if (HitBoxComponent != nullptr)
	{
		HitBoxComponent->ResetProperties();
		//HitBoxComponent->SetDebug(true);
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
	if (AnimInstance != nullptr && DeathAnimSeq != nullptr)
	{
		Dying = true;
		AnimInstance->PlayAnimation(DeathAnimSeq, 0.25f, 0.25f, 0.5f);
	}
}

float ACBoss::GetOpponentDistance()
{
	AActor* Target = AIController ? AIController->GetCurrentAttackTargetActor() : nullptr;
	return Target ? FVector::Dist(Target->GetActorLocation(), GetActorLocation()) : 0.f;
}

void ACBoss::OverrideNextTickCombo(int32 NextAction, bool bIgnoreCooldown, bool bCancleDelay)
{
	if (AIController != nullptr)
	{
		AIController->OverrideNextTickCombo(NextAction, bIgnoreCooldown);
		if (AnimInstance != nullptr && bCancleDelay)AnimInstance->SetBusy(false);
	}
}

FOnStatusChanged* ACBoss::GetStatusChanagedDelegate()
{
	return StatComponent != nullptr? &StatComponent->OnStatusChanged : nullptr;
}

FOnGroggyEnd* ACBoss::GetGroggyEndDelegate()
{
	return &OnGroggyEndDelegate;
}

bool ACBoss::IsExecutable()
{
	return StatComponent != nullptr ? StatComponent->IsGroggy() && StatComponent->GetExecutableCount() > 0 : false;
}

void ACBoss::UseExecutableCount()
{
	if (StatComponent != nullptr)
	{
		int32 NewEC = StatComponent->GetExecutableCount() - 1;
		StatComponent->SetExecutableCount(NewEC);
		if (NewEC <= 0) OnGroggyExecuteCountAllOut.Broadcast();
	}
}

void ACBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Dying_Submerge)
	{
		GetMesh()->SetRelativeLocation(GetMesh()->GetRelativeLocation() - FVector(0.f, 0.f, 50.f * DeltaTime));
	}

	TimeSeriesData_EnemyLocation.AddTail(GetActorLocation());
	if (TimeSeriesData_EnemyLocation.Num() > 10)
	{
		TimeSeriesData_EnemyLocation.RemoveNode(TimeSeriesData_EnemyLocation.GetHead());
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

