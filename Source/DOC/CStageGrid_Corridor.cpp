#include "CStageGrid_Corridor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

ACStageGrid_Corridor::ACStageGrid_Corridor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor1Finder(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-01.SM-Floor-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor2Finder(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-02.SM-Floor-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor3Finder(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-03.SM-Floor-03"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor4Finder(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-04.SM-Floor-04"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor5Finder(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-05.SM-Floor-05"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallFinder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-01-01.SM-Wall-01-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallBaseFinder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-01-02.SM-Wall-01-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallClamp1Finder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-04-01.SM-Wall-04-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallClmap1BaseFinder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-04-02.SM-Wall-04-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallDoorFinder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-02-01.SM-Wall-02-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallDoorBaseFinder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-02-02.SM-Wall-02-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Philar1Finder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Pillar-01.SM-Pillar-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Philar2Finder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Pillar-02.SM-Pillar-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Ceiling1Finder(TEXT("/Game/Dungeon/Meshes/Ceiling/SM-Ceiling-01.SM-Ceiling-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Ceiling2Finder(TEXT("/Game/Dungeon/Meshes/Ceiling/SM-Ceiling-02.SM-Ceiling-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Ceiling3Finder(TEXT("/Game/Dungeon/Meshes/Ceiling/SM-Ceiling-03.SM-Ceiling-03"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CeilingArchWallFinder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-03.SM-Wall-03"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CeilingArchFinder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Arch-01.SM-Arch-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CandleFinder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Candle-01.SM-Candle-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CandlelabraFinder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Candelabra-04.SM-Candelabra-04"));
	//ConstructorHelpers::FObjectFinder<UStaticMesh> CandleFlameFinder(TEXT("/Game/Dungeon/Meshes/Prop/SM-CandleFlame-01.SM-CandleFlame-01"));

	SceneComponent = CreateDefaultSubobject<USceneComponent>(FName("SceneComponent"));
	HISM_Floor1 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Floor1"));
	HISM_Floor2 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Floor2"));
	HISM_Floor3 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Floor3"));
	HISM_Floor4 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Floor4"));
	HISM_Floor5 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Floor5"));
	HISM_Wall = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Wall"));
	HISM_Wall_Base = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Wall_Base"));
	HISM_Wall_Clamp1 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Wall_Clamp1"));
	HISM_Wall_Clamp1_Base = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Wall_Clamp1_Base"));
	HISM_Wall_Door = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Wall_Door"));
	HISM_Wall_Door_Base = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Wall_Door_Base"));
	HISM_Philar1 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Philar1"));
	HISM_Philar2 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Philar2"));
	HISM_Ceiling1 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Ceiling1"));
	HISM_Ceiling2 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Ceiling2"));
	HISM_Ceiling3 = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Ceiling3"));
	HISM_CeilingArchWall = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_CeilingArchWall"));
	HISM_CeilingArch = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_CeilingArch"));
	HISM_Candle = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Candle"));
	HISM_Candlelabra = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_Candlelabra"));
	//HISM_CandleFlame = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HISM_CandleFlame"));

	HISM_Arr.Add(HISM_Floor1);
	HISM_Arr.Add(HISM_Floor2);
	HISM_Arr.Add(HISM_Floor3);
	HISM_Arr.Add(HISM_Floor4);
	HISM_Arr.Add(HISM_Floor5);
	HISM_Arr.Add(HISM_Wall);
	HISM_Arr.Add(HISM_Wall_Base);
	HISM_Arr.Add(HISM_Wall_Clamp1);
	HISM_Arr.Add(HISM_Wall_Clamp1_Base);
	HISM_Arr.Add(HISM_Wall_Door);
	HISM_Arr.Add(HISM_Wall_Door_Base);
	HISM_Arr.Add(HISM_Philar1);
	HISM_Arr.Add(HISM_Philar2);
	HISM_Arr.Add(HISM_Ceiling1);
	HISM_Arr.Add(HISM_Ceiling2);
	HISM_Arr.Add(HISM_Ceiling3);
	HISM_Arr.Add(HISM_CeilingArchWall);
	HISM_Arr.Add(HISM_CeilingArch);
	HISM_Arr.Add(HISM_Candle);
	HISM_Arr.Add(HISM_Candlelabra);
	//HISM_Arr.Add(HISM_CandleFlame);

	SetRootComponent(SceneComponent);
	HISM_Floor1->SetupAttachment(SceneComponent);
	HISM_Floor2->SetupAttachment(SceneComponent);
	HISM_Floor3->SetupAttachment(SceneComponent);
	HISM_Floor4->SetupAttachment(SceneComponent);
	HISM_Floor5->SetupAttachment(SceneComponent);
	HISM_Wall->SetupAttachment(SceneComponent);
	HISM_Wall_Base->SetupAttachment(SceneComponent);
	HISM_Wall_Clamp1->SetupAttachment(SceneComponent);
	HISM_Wall_Clamp1_Base->SetupAttachment(SceneComponent);
	HISM_Wall_Door->SetupAttachment(SceneComponent);
	HISM_Wall_Door_Base->SetupAttachment(SceneComponent);
	HISM_Philar1->SetupAttachment(SceneComponent);
	HISM_Philar2->SetupAttachment(SceneComponent);
	HISM_Ceiling1->SetupAttachment(SceneComponent);
	HISM_Ceiling2->SetupAttachment(SceneComponent);
	HISM_Ceiling3->SetupAttachment(SceneComponent);
	HISM_CeilingArchWall->SetupAttachment(SceneComponent);
	HISM_CeilingArch->SetupAttachment(SceneComponent);
	HISM_Candle->SetupAttachment(SceneComponent);
	HISM_Candlelabra->SetupAttachment(SceneComponent);
	//HISM_CandleFlame->SetupAttachment(SceneComponent);

	if (Floor1Finder.Succeeded() && HISM_Floor1 != nullptr)
	{
		SM_Floor1 = Floor1Finder.Object;
		HISM_Floor1->SetStaticMesh(SM_Floor1);
	}
	if (Floor2Finder.Succeeded() && HISM_Floor2 != nullptr)
	{
		SM_Floor2 = Floor2Finder.Object;
		HISM_Floor2->SetStaticMesh(SM_Floor2);
	}
	if (Floor3Finder.Succeeded() && HISM_Floor3 != nullptr)
	{
		SM_Floor3 = Floor3Finder.Object;
		HISM_Floor3->SetStaticMesh(SM_Floor3);
	}
	if (Floor4Finder.Succeeded() && HISM_Floor4 != nullptr)
	{
		SM_Floor4 = Floor4Finder.Object;
		HISM_Floor4->SetStaticMesh(SM_Floor4);
	}
	if (Floor5Finder.Succeeded() && HISM_Floor5 != nullptr)
	{
		SM_Floor5 = Floor5Finder.Object;
		HISM_Floor5->SetStaticMesh(SM_Floor5);
	}
	if (WallFinder.Succeeded() && HISM_Wall != nullptr)
	{
		SM_Wall = WallFinder.Object;
		HISM_Wall->SetStaticMesh(SM_Wall);
	}
	if (WallBaseFinder.Succeeded() && HISM_Wall_Base != nullptr)
	{
		SM_Wall_Base = WallBaseFinder.Object;
		HISM_Wall_Base->SetStaticMesh(SM_Wall_Base);
	}
	if (WallClamp1Finder.Succeeded() && HISM_Wall_Clamp1 != nullptr)
	{
		SM_Wall_Clamp1 = WallClamp1Finder.Object;
		HISM_Wall_Clamp1->SetStaticMesh(SM_Wall_Clamp1);
	}
	if (WallClmap1BaseFinder.Succeeded() && HISM_Wall_Clamp1_Base != nullptr)
	{
		SM_Wall_Clamp1_Base = WallClmap1BaseFinder.Object;
		HISM_Wall_Clamp1_Base->SetStaticMesh(SM_Wall_Clamp1_Base);
	}
	if (WallDoorFinder.Succeeded() && HISM_Wall != nullptr)
	{
		SM_Wall_Door = WallDoorFinder.Object;
		HISM_Wall_Door->SetStaticMesh(SM_Wall_Door);
	}
	if (WallDoorBaseFinder.Succeeded() && HISM_Wall_Base != nullptr)
	{
		SM_Wall_Door_Base = WallDoorBaseFinder.Object;
		HISM_Wall_Door_Base->SetStaticMesh(SM_Wall_Door_Base);
	}
	if (Philar1Finder.Succeeded() && HISM_Philar1 != nullptr)
	{
		SM_Philar1 = Philar1Finder.Object;
		HISM_Philar1->SetStaticMesh(SM_Philar1);
	}
	if (Philar2Finder.Succeeded() && HISM_Philar2 != nullptr)
	{
		SM_Philar2 = Philar2Finder.Object;
		HISM_Philar2->SetStaticMesh(SM_Philar2);
	}
	if (Ceiling1Finder.Succeeded() && HISM_Ceiling1 != nullptr)
	{
		SM_Ceiling1 = Ceiling1Finder.Object;
		HISM_Ceiling1->SetStaticMesh(SM_Ceiling1);
	}
	if (Ceiling2Finder.Succeeded() && HISM_Ceiling2 != nullptr)
	{
		SM_Ceiling2 = Ceiling2Finder.Object;
		HISM_Ceiling2->SetStaticMesh(SM_Ceiling2);
	}
	if (Ceiling3Finder.Succeeded() && HISM_Ceiling2 != nullptr)
	{
		SM_Ceiling3 = Ceiling3Finder.Object;
		HISM_Ceiling3->SetStaticMesh(SM_Ceiling3);
	}
	if (CeilingArchWallFinder.Succeeded() && HISM_CeilingArchWall != nullptr)
	{
		SM_CeilingArchWall = CeilingArchWallFinder.Object;
		HISM_CeilingArchWall->SetStaticMesh(SM_CeilingArchWall);
	}
	if (CeilingArchFinder.Succeeded() && HISM_CeilingArch != nullptr)
	{
		SM_CeilingArch = CeilingArchFinder.Object;
		HISM_CeilingArch->SetStaticMesh(SM_CeilingArch);
	}

	if (CandleFinder.Succeeded() && HISM_Candle != nullptr)
	{
		SM_Candle = CandleFinder.Object;
		HISM_Candle->SetStaticMesh(SM_Candle);
	}
	if (CandlelabraFinder.Succeeded() && HISM_Candlelabra != nullptr)
	{
		SM_Candlelabra = CandlelabraFinder.Object;
		HISM_Candlelabra->SetStaticMesh(SM_Candlelabra);
	}
}

void ACStageGrid_Corridor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACStageGrid_Corridor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACStageGrid_Corridor::AddGrid(FTransform Transform, int32 Type, bool bWorldSpace)
{
	float temp = 0.f;
	int32 D = 0;
	int32 tempx = 1;
	int32 tempy = 0;
	switch (Type)
	{
	case(STAGE_GRID_CORRIDOR_FLOOR):
		temp = FMath::RandRange(0.f, 3.f);
		if (temp < 1.f) HISM_Floor1->AddInstance(Transform, bWorldSpace);
		else if (temp < 2.f) HISM_Floor2->AddInstance(Transform, bWorldSpace);
		else HISM_Floor3->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_WALL):
		HISM_Wall->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_WALL_BASE):
		HISM_Wall_Base->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_WALL_DOOR):
		HISM_Wall_Door->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_WALL_DOOR_BASE):
		HISM_Wall_Door_Base->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_PHILAR):
		temp = FMath::RandRange(0.f, 1.f);
		if (temp < 0.85f) HISM_Philar1->AddInstance(Transform, bWorldSpace);
		else HISM_Philar2->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_CEILING1):
		HISM_Ceiling1->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_CEILING2):
		HISM_Ceiling2->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_CEILING3):
		HISM_Ceiling3->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL):
		HISM_CeilingArchWall->AddInstance(Transform, bWorldSpace);
		HISM_CeilingArch->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_FLOOR_CLAMP):
		HISM_Floor4->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_WALL_CLAMP1):
		HISM_Wall_Clamp1->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE):
		HISM_Wall_Clamp1_Base->AddInstance(Transform, bWorldSpace);
		break;
	case(STAGE_GRID_CORRIDOR_CANDLELABRA):
		HISM_Candlelabra->AddInstance(Transform, bWorldSpace);
		D = ((int32)((FMath::CeilToInt32(Transform.GetRotation().Rotator().Yaw) + 360
			) % 360
			) / 90
			) % 4;
		tempx = (D % 2 == 0 ? 0 : (D < 2 ? -1 : 1));
		tempy = (D % 2 > 0 ? 0 : (D < 1 ? 1 : -1));;

		HISM_Candle->AddInstance(FTransform(
			FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
			Transform.GetLocation() + FVector(tempx * 72.f, tempy * 72.f, 24.f)), bWorldSpace);
		HISM_Candle->AddInstance(FTransform(
			FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
			Transform.GetLocation() + FVector(tempx * 54.f, tempy * 54.f, 85.f)), bWorldSpace);
		break;
	default:
		break;
	}
}

void ACStageGrid_Corridor::ClearGrid()
{
	for (UHierarchicalInstancedStaticMeshComponent* hism: HISM_Arr)
	{
		hism->ClearInstances();
	}
}

