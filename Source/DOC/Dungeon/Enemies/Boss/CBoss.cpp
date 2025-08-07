#include "CBoss.h"
#include "Player/UI/CMonsterHP.h"
#include "GameSystem/CStatComponent.h"
#include "GameSystem/CHitBoxComponent.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IAnimInstance.h"
#include "NiagaraComponent.h"

ACBoss::ACBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshMelleeFinder(TEXT("/Game/Dungeon/Boss/Buff_Red/Meshes/Buff_Red.Buff_Red"));
	if (SKMeshMelleeFinder.Succeeded()) GetMesh()->SetSkeletalMesh(SKMeshMelleeFinder.Object);
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPMelleeFinder(TEXT("/Game/Dungeon/Boss/AnimBP_Boss"));
	if (AnimBPMelleeFinder.Succeeded()) AnimClass_Boss = AnimBPMelleeFinder.Class;
	HitBoxComponent = CreateDefaultSubobject<UCHitBoxComponent>(TEXT("HitBoxComponent"));
	AutoPossessAI = EAutoPossessAI::Disabled;
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

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimClass(AnimClass_Boss);
}

void ACBoss::BeginPlay()
{
	Super::BeginPlay();
	MonsterHPComponent->SetVisibility(false);

	if (StatComponent && MonsterHPComponent)
	{
		MonsterHPComponent->SetDelegates(&StatComponent->OnStatusChanged);
		StatComponent->OnDeath.AddUFunction(this, TEXT("Died"));
		StatComponent->SetupDelegates(&OnReceivedDamageDelegate);
	}
	
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

void ACBoss::Died(FDamageConfig DamageConfig)
{
}

void ACBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

