#include "CMinion.h"

ACMinion::ACMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh>SkeletalMeshFinder(TEXT("/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Melee_Core_Dawn.Minion_Lane_Melee_Core_Dawn"));
	ConstructorHelpers::FClassFinder<UAnimInstance>AnimBPFinder(TEXT("/Game/Dungeon/Minion/AnimBP_Minion"));
	if (SkeletalMeshFinder.Succeeded()) GetMesh()->SetSkeletalMesh(SkeletalMeshFinder.Object);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	if (AnimBPFinder.Succeeded()) GetMesh()->SetAnimClass(AnimBPFinder.Class);
}

void ACMinion::BeginPlay()
{
	Super::BeginPlay();
}

void ACMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

