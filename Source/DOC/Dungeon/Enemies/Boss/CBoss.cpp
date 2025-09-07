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

	AnimSeqArr.SetNum(ENEMYCHARACTER_ACTIONTYPE_NUM);
	ConstructorHelpers::FObjectFinder<UAnimSequence> MelleAttackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_Punch_01.Attack_Punch_01"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> RangedAttackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_Uppercut.Attack_Uppercut"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> ChargeFinder		(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Knock_Bwd.Knock_Bwd"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> JumpChargeFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_BigSmash.Attack_BigSmash"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> ComboAttackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_SmallCombo.Attack_SmallCombo"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> HeavyAttackFinder	(TEXT("/Game/Dungeon/Boss/Buff_Red/Animations/Attack_Punch_02.Attack_Punch_02"));

	if (MelleAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_MELLEEATTACK] = MelleAttackFinder.Object;
	if (RangedAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_RANGEDATTACK] = RangedAttackFinder.Object;
	if (ChargeFinder.Succeeded())		AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_CHARGE] = ChargeFinder.Object;
	if (JumpChargeFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_JUMPCHARGE] = JumpChargeFinder.Object;
	if (ComboAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_COMBO_ATTACK] = ComboAttackFinder.Object;
	if (HeavyAttackFinder.Succeeded())	AnimSeqArr[ENEMYCHARACTER_ACTIONTYPE_HEAVY_ATTACK] = HeavyAttackFinder.Object;

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimClass(AnimClass_Boss);

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
}

void ACBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACBoss::OnEnemyAction(int32 ActionType)
{
	UE_LOG(LogTemp, Log, TEXT("ACBoss : OnEnemyAction ActionType : %d"), ActionType);
	if (!AnimSeqArr.IsValidIndex(ActionType) || AnimInstance == nullptr) return;
	AnimInstance->PlayAnimation(AnimSeqArr[ActionType]);
}

