// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeon/CNPC.h"

// Sets default values
ACNPC::ACNPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACNPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

