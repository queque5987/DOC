#include "CGameState_Stage.h"
#include "Components/PointLightComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"
#include "Components/BrushComponent.h"
#include "EngineUtils.h"
#include "Dungeon/CGeneratedStage.h"
#include "Dungeon/InteractableActors/CChest.h"
#include "Interfaces/IInteractableItem.h"
#include "Dungeon/InteractableActors/CPotion.h"
#include "Dungeon/InteractableActors/CGemstone.h"
#include "Dungeon/InteractableActors/CDoor.h"
#include "Player/UI/CItemData.h"
#include "Dungeon/InteractableActors/CBrazier.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Dungeon/Enemies/Minion/CMinion.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/Equipment/CSword.h"
#include "Interfaces/CStageStructs.h"
#include "CProjectile.h"
#include "Player/UI/CDamage.h"
#include "GameSystem/CSubsystem_ItemManager.h"
#include "GameSystem/CGameMode_Stage.h"
#include "Components/SplineComponent.h"

ACGameState_Stage::ACGameState_Stage() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponents.SetNum(	STAGE_GRID_CORRIDOR_NUM);
	StaticMeshes_Available.SetNum(	STAGE_GRID_CORRIDOR_NUM);
	StaticMeshes.SetNum(			STAGE_GRID_CORRIDOR_NUM);
	ItemClasses.SetNum(				INTERACTABLE_ITEM_NUM);
	Item_Available.SetNum(			INTERACTABLE_ITEM_NUM);
	Items.SetNum(					INTERACTABLE_ITEM_NUM);
	EnemyClasses.SetNum(			ENEMYCHARACTER_NUM);
	Enemies.SetNum(					ENEMYCHARACTER_NUM);
	Enemy_Available.SetNum(			ENEMYCHARACTER_NUM);
	ParticleSystems.SetNum(			PARTICLE_NUM);
	EquipmentsClasses.SetNum(		EQUIPMENT_NUM);
	Equipments.SetNum(				EQUIPMENT_NUM);
	Equipments_Available.SetNum(	EQUIPMENT_NUM);

	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor1Finder			(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-01.SM-Floor-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor2Finder			(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-02.SM-Floor-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor3Finder			(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-03.SM-Floor-03"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Floor4Finder			(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-04.SM-Floor-04"));
	//ConstructorHelpers::FObjectFinder<UStaticMesh> Floor5Finder		(TEXT("/Game/Dungeon/Meshes/Floor/SM-Floor-05.SM-Floor-05")); ?
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallFinder			(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-01-01.SM-Wall-01-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallBaseFinder		(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-01-02.SM-Wall-01-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallClamp1Finder		(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-04-01.SM-Wall-04-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallClmap1BaseFinder	(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-04-02.SM-Wall-04-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallDoorFinder		(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-02-01.SM-Wall-02-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> WallDoorBaseFinder	(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-02-02.SM-Wall-02-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Philar1Finder		(TEXT("/Game/Dungeon/Meshes/Wall/SM-Pillar-01.SM-Pillar-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Philar2Finder		(TEXT("/Game/Dungeon/Meshes/Wall/SM-Pillar-02.SM-Pillar-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Ceiling1Finder		(TEXT("/Game/Dungeon/Meshes/Ceiling/SM-Ceiling-01.SM-Ceiling-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Ceiling2Finder		(TEXT("/Game/Dungeon/Meshes/Ceiling/SM-Ceiling-02.SM-Ceiling-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Ceiling3Finder		(TEXT("/Game/Dungeon/Meshes/Ceiling/SM-Ceiling-03.SM-Ceiling-03"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CeilingArchWallFinder(TEXT("/Game/Dungeon/Meshes/Wall/SM-Wall-03.SM-Wall-03"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CeilingArchFinder	(TEXT("/Game/Dungeon/Meshes/Wall/SM-Arch-01.SM-Arch-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CandleFinder			(TEXT("/Game/Dungeon/Meshes/Prop/SM-Candle-01.SM-Candle-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CandlelabraFinder	(TEXT("/Game/Dungeon/Meshes/Prop/SM-Candelabra-04.SM-Candelabra-04"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CandleFlameFinder	(TEXT("/Game/Dungeon/Meshes/Prop/SM-CandleFlame-01.SM-CandleFlame-01"));
	
	if (Floor1Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_FLOOR1] = Floor1Finder.Object;
	if (Floor2Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_FLOOR2] = Floor2Finder.Object;
	if (Floor3Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_FLOOR3] = Floor3Finder.Object;
	if (Floor4Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_FLOOR_CLAMP] = Floor4Finder.Object;
	if (WallFinder.Succeeded())					StaticMeshes[STAGE_GRID_CORRIDOR_WALL] = WallFinder.Object;
	if (WallBaseFinder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_WALL_BASE] = WallBaseFinder.Object;
	if (WallClamp1Finder.Succeeded())			StaticMeshes[STAGE_GRID_CORRIDOR_WALL_CLAMP1] = WallClamp1Finder.Object;
	if (WallClmap1BaseFinder.Succeeded())		StaticMeshes[STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE] = WallClmap1BaseFinder.Object;
	if (WallDoorFinder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_WALL_DOOR] = WallDoorFinder.Object;
	if (WallDoorBaseFinder.Succeeded())			StaticMeshes[STAGE_GRID_CORRIDOR_WALL_DOOR_BASE] = WallDoorBaseFinder.Object;
	if (Philar1Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_PHILAR1] = Philar1Finder.Object;
	if (Philar2Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_PHILAR2] = Philar2Finder.Object;
	if (Ceiling1Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_CEILING1] = Ceiling1Finder.Object;
	if (Ceiling2Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_CEILING2] = Ceiling2Finder.Object;
	if (Ceiling3Finder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_CEILING3] = Ceiling3Finder.Object;
	if (CeilingArchWallFinder.Succeeded())		StaticMeshes[STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL] = CeilingArchWallFinder.Object;
	if (CeilingArchFinder.Succeeded())			StaticMeshes[STAGE_GRID_CORRIDOR_CEILING_ARCH] = CeilingArchFinder.Object;
	if (CandleFinder.Succeeded())				StaticMeshes[STAGE_GRID_CORRIDOR_CANDLE] = CandleFinder.Object;
	if (CandlelabraFinder.Succeeded())			StaticMeshes[STAGE_GRID_CORRIDOR_CANDLELABRA] = CandlelabraFinder.Object;
	if (CandleFlameFinder.Succeeded())			StaticMeshes[STAGE_GRID_CORRIDOR_CANDLEFLAME] = CandleFlameFinder.Object;

	if (Floor1Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Floor1Finder.Object, {Floor1Finder.Object->GetMaterial(0), Floor1Finder.Object->GetMaterial(1) });
	if (Floor2Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Floor2Finder.Object, {Floor2Finder.Object->GetMaterial(0), Floor2Finder.Object->GetMaterial(1) });
	if (Floor3Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Floor3Finder.Object, {Floor3Finder.Object->GetMaterial(0), Floor3Finder.Object->GetMaterial(1) });
	if (Floor4Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Floor4Finder.Object, {Floor4Finder.Object->GetMaterial(0), Floor4Finder.Object->GetMaterial(1) });
	if (WallFinder.Succeeded())					StaticMeshOpaqueMaterialInstance.Add(WallFinder.Object, {WallFinder.Object->GetMaterial(0)});
	if (WallBaseFinder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(WallBaseFinder.Object, {WallBaseFinder.Object->GetMaterial(0)});
	if (WallClamp1Finder.Succeeded())			StaticMeshOpaqueMaterialInstance.Add(WallClamp1Finder.Object, {WallClamp1Finder.Object->GetMaterial(0)});
	if (WallClmap1BaseFinder.Succeeded())		StaticMeshOpaqueMaterialInstance.Add(WallClmap1BaseFinder.Object, {WallClmap1BaseFinder.Object->GetMaterial(0)});
	if (WallDoorFinder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(WallDoorFinder.Object, {WallDoorFinder.Object->GetMaterial(0)});
	if (WallDoorBaseFinder.Succeeded())			StaticMeshOpaqueMaterialInstance.Add(WallDoorBaseFinder.Object, {WallDoorBaseFinder.Object->GetMaterial(0)});
	if (Philar1Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Philar1Finder.Object, {Philar1Finder.Object->GetMaterial(0)});
	if (Philar2Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Philar2Finder.Object, {Philar2Finder.Object->GetMaterial(0)});
	if (Ceiling1Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Ceiling1Finder.Object, {Ceiling1Finder.Object->GetMaterial(0)});
	if (Ceiling2Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Ceiling2Finder.Object, {Ceiling2Finder.Object->GetMaterial(0)});
	if (Ceiling3Finder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(Ceiling3Finder.Object, {Ceiling3Finder.Object->GetMaterial(0)});
	if (CeilingArchWallFinder.Succeeded())		StaticMeshOpaqueMaterialInstance.Add(CeilingArchWallFinder.Object, {CeilingArchWallFinder.Object->GetMaterial(0)});
	if (CeilingArchFinder.Succeeded())			StaticMeshOpaqueMaterialInstance.Add(CeilingArchFinder.Object, {CeilingArchFinder.Object->GetMaterial(0)});
	if (CandleFinder.Succeeded())				StaticMeshOpaqueMaterialInstance.Add(CandleFinder.Object, {CandleFinder.Object->GetMaterial(0)});
	if (CandlelabraFinder.Succeeded())			StaticMeshOpaqueMaterialInstance.Add(CandlelabraFinder.Object, { CandlelabraFinder.Object->GetMaterial(0), CandlelabraFinder.Object->GetMaterial(1) });
	if (CandleFlameFinder.Succeeded())			StaticMeshOpaqueMaterialInstance.Add(CandleFlameFinder.Object, { CandleFlameFinder.Object->GetMaterial(0) });

	ConstructorHelpers::FObjectFinder<UMaterialInterface> Floor0_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Floor-01-02.MI-Floor-01-02"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Floor1_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Floor-01-01.MI-Floor-01-01"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Floor2_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Floor-02-01.MI-Floor-02-01"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Floor3_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Floor-03-01.MI-Floor-03-01"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> WallBase_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Wall-01-02.MI-Wall-01-02"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Wall_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Wall-01-01.MI-Wall-01-01"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> WallDoor_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Wall-02-01.MI-Wall-02-01"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> WallDoorBase_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Wall-02-02.MI-Wall-02-02"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Phillar1_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Pillar-01.MI-Pillar-01"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Phillar2_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Pillar-02.MI-Pillar-02"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Ceiling1_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Ceiling-01.MI-Ceiling-01"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Ceiling2_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Ceiling-02.MI-Ceiling-02"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> Candle_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Candle-01.MI-Candle-01"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> CandleLabra0_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Candelabra-04-02.MI-Candelabra-04-02"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> CandleLabra1_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Candelabra-04-01.MI-Candelabra-04-01"));

	ConstructorHelpers::FObjectFinder<UMaterialInterface> CeilingArchWall_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Wall-03.MI-Wall-03"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> CeilingArch_TranslucentMIFinder(TEXT("/Game/Dungeon/Materials/MaterialInstances/Translucents/MI-Arch-01.MI-Arch-01"));

	if (Floor1Finder.Succeeded() && (Floor0_TranslucentMIFinder.Succeeded() && Floor1_TranslucentMIFinder.Succeeded())) StaticMeshTranslucentMaterialInstance.Add(Floor1Finder.Object, { Floor0_TranslucentMIFinder.Object, Floor1_TranslucentMIFinder.Object });
	if (Floor2Finder.Succeeded() && (Floor0_TranslucentMIFinder.Succeeded() && Floor2_TranslucentMIFinder.Succeeded())) StaticMeshTranslucentMaterialInstance.Add(Floor1Finder.Object, { Floor0_TranslucentMIFinder.Object, Floor2_TranslucentMIFinder.Object });
	if (Floor3Finder.Succeeded() && (Floor0_TranslucentMIFinder.Succeeded() && Floor3_TranslucentMIFinder.Succeeded())) StaticMeshTranslucentMaterialInstance.Add(Floor1Finder.Object, { Floor0_TranslucentMIFinder.Object, Floor3_TranslucentMIFinder.Object });
	if (WallFinder.Succeeded() && Wall_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(WallFinder.Object, { Wall_TranslucentMIFinder.Object });
	if (WallBaseFinder.Succeeded() && WallBase_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(WallBaseFinder.Object, { WallBase_TranslucentMIFinder.Object });
	if (WallDoorFinder.Succeeded() && WallDoor_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(WallDoorFinder.Object, { WallDoor_TranslucentMIFinder.Object });
	if (WallDoorBaseFinder.Succeeded() && WallDoorBase_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(WallDoorBaseFinder.Object, { WallDoorBase_TranslucentMIFinder.Object });
	if (Philar1Finder.Succeeded() && Phillar1_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(Philar1Finder.Object, { Phillar1_TranslucentMIFinder.Object });
	if (Philar2Finder.Succeeded() && Phillar2_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(Philar2Finder.Object, { Phillar2_TranslucentMIFinder.Object });
	if (Ceiling1Finder.Succeeded() && Ceiling1_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(Ceiling1Finder.Object, { Ceiling1_TranslucentMIFinder.Object });
	if (Ceiling2Finder.Succeeded() && Ceiling2_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(Ceiling2Finder.Object, { Ceiling2_TranslucentMIFinder.Object });
	if (CandleFinder.Succeeded() && Candle_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(Ceiling2Finder.Object, { Candle_TranslucentMIFinder.Object });
	if (CandlelabraFinder.Succeeded() && (CandleLabra0_TranslucentMIFinder.Succeeded() && CandleLabra1_TranslucentMIFinder.Succeeded())) StaticMeshTranslucentMaterialInstance.Add(CandlelabraFinder.Object, { CandleLabra0_TranslucentMIFinder.Object, CandleLabra1_TranslucentMIFinder.Object });

	if (CeilingArchWallFinder.Succeeded() && CeilingArchWall_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(CeilingArchWallFinder.Object, { CeilingArchWall_TranslucentMIFinder.Object });
	if (CeilingArchFinder.Succeeded() && CeilingArch_TranslucentMIFinder.Succeeded()) StaticMeshTranslucentMaterialInstance.Add(CeilingArchFinder.Object, { CeilingArch_TranslucentMIFinder.Object });

	ItemClasses[INTERACTABLE_ITEM_POTION_BLUE]		 = ACPotion::StaticClass();
	ItemClasses[INTERACTABLE_ITEM_POTION_GREEN]		 = ACPotion::StaticClass();
	//ItemClasses[INTERACTABLE_ITEM_GEMSTONE_BLUE]	 = ACGemstone::StaticClass();
	//ItemClasses[INTERACTABLE_ITEM_GEMSTONE_YELLOW]	 = ACGemstone::StaticClass();
	//ItemClasses[INTERACTABLE_ITEM_GEMSTONE_RED]		 = ACGemstone::StaticClass();
	//ItemClasses[INTERACTABLE_ITEM_GEMSTONE_GREEN]	 = ACGemstone::StaticClass();
	//ItemClasses[INTERACTABLE_ITEM_GEMSTONE_PURPLE]	 = ACGemstone::StaticClass();
	//ItemClasses[INTERACTABLE_ITEM_GEMSTONE_PINK]	 = ACGemstone::StaticClass();

	EnemyClasses[ENEMYCHARACTER_MINION] = ACMinion::StaticClass();

	ConstructorHelpers::FObjectFinder<UParticleSystem> MinionSpawnFinder(TEXT("/Game/Dungeon/Minion/Particles/Minions/Shared/P_MinionSpawn.P_MinionSpawn"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> MinionMelleeHitImpactFinder(TEXT("/Game/Dungeon/Minion/Particles/Minions/Shared/P_Minion_Melee_Impact.P_Minion_Melee_Impact"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> MinionRangedProjectileFinder(TEXT("/Game/Dungeon/Minion/Particles/Minions/P_Prime_Helix_SpecialAttack1_Projectile.P_Prime_Helix_SpecialAttack1_Projectile"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> PlayerHitImpactFinder(TEXT("/Game/Dungeon/Minion/Particles/Minions/Shared/P_Minion_Melee_Impact2.P_Minion_Melee_Impact2"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> PlayerCounterSucceededFinder(TEXT("/Game/Dungeon/Minion/Particles/SharedGameplay/States/Recall/P_PortalStone_BigBeamDown.P_PortalStone_BigBeamDown"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> MinionDeadRecallFinder(TEXT("/Game/Dungeon/Minion/Particles/SharedGameplay/States/Recall/P_Recall_BigBeam_UP.P_Recall_BigBeam_UP"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> PotionUseFinder(TEXT("/Game/Dungeon/Minion/Particles/PlayerBuffs/P_CarriedBuff_Received_Red.P_CarriedBuff_Received_Red"));
	
	if (MinionSpawnFinder.Succeeded())				ParticleSystems[PARTICLE_MINION_SPAWN] = MinionSpawnFinder.Object;
	if (MinionMelleeHitImpactFinder.Succeeded())	ParticleSystems[PARTICLE_MINION_MELLEE_HIT_IMPACT] = MinionMelleeHitImpactFinder.Object;
	if (MinionRangedProjectileFinder.Succeeded())	ParticleSystems[PARTICLE_MINION_RANGED_PROJECTILE] = MinionRangedProjectileFinder.Object;
	if (PlayerHitImpactFinder.Succeeded())			ParticleSystems[PARTICLE_PLAYER_HIT_MELLEE_IMPACT] = PlayerHitImpactFinder.Object;
	if (PlayerCounterSucceededFinder.Succeeded())	ParticleSystems[PARTICLE_PLAYER_HIT_COUNTER_SUCCEEDED] = PlayerCounterSucceededFinder.Object;
	if (MinionDeadRecallFinder.Succeeded())			ParticleSystems[PARTICLE_MINION_DEAD_RECALL] = MinionDeadRecallFinder.Object;
	if (PotionUseFinder.Succeeded())				ParticleSystems[PARTICLE_PLAYER_POTION_USE] = PotionUseFinder.Object;

	EquipmentsClasses[EQUIPMENT_SWORD] = ACSword::StaticClass();

	ConstructorHelpers::FClassFinder<AActor> GloveFinder(TEXT("/Game/Player/Equipment/Class/BP_Wearable_Glove"));
	ConstructorHelpers::FClassFinder<AActor> MaskFinder(TEXT("/Game/Player/Equipment/Class/BP_Wearable_Mask"));
	ConstructorHelpers::FClassFinder<AActor> PantsFinder(TEXT("/Game/Player/Equipment/Class/BP_Wearable_Pants"));
	ConstructorHelpers::FClassFinder<AActor> ShoesFinder(TEXT("/Game/Player/Equipment/Class/BP_Wearable_Shoes"));
	ConstructorHelpers::FClassFinder<AActor> Torso1Finder(TEXT("/Game/Player/Equipment/Class/BP_Wearable_Torso1"));
	ConstructorHelpers::FClassFinder<AActor> Torso2Finder(TEXT("/Game/Player/Equipment/Class/BP_Wearable_Torso2"));
	ConstructorHelpers::FClassFinder<AActor> Torso3Finder(TEXT("/Game/Player/Equipment/Class/BP_Wearable_Torso3"));

	if (GloveFinder.Succeeded()) EquipmentsClasses[EQUIPMENT_GLOVE] = GloveFinder.Class;
	if (MaskFinder.Succeeded()) EquipmentsClasses[EQUIPMENT_HELMET] = MaskFinder.Class;
	if (PantsFinder.Succeeded()) EquipmentsClasses[EQUIPMENT_PANTS] = PantsFinder.Class;
	if (ShoesFinder.Succeeded()) EquipmentsClasses[EQUIPMENT_SHOSE] = ShoesFinder.Class;
	if (Torso1Finder.Succeeded()) EquipmentsClasses[EQUIPMENT_TORSO1] = Torso1Finder.Class;
	if (Torso2Finder.Succeeded()) EquipmentsClasses[EQUIPMENT_TORSO2] = Torso2Finder.Class;
	if (Torso3Finder.Succeeded()) EquipmentsClasses[EQUIPMENT_TORSO3] = Torso3Finder.Class;
}

void ACGameState_Stage::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACGameState_Stage::HandleBeginPlay()
{
	Super::HandleBeginPlay();
}

void ACGameState_Stage::BeginPlay()
{
	Super::BeginPlay();

	ACGameMode_Stage* StageMode = Cast<ACGameMode_Stage>(GetWorld()->GetAuthGameMode());
	if (StageMode != nullptr)
	{
		StageClearedDelegatePtr = StageMode->GetStageClearedDelegatePtr();
	}

	GenerateNextStage();

	IIEnemyCharacter* EC = GetEnemyCharacter(this, ENEMYCHARACTER_MINION, FTransform(FVector(3100.f, 2119.f, 600.f)));
	if (EC != nullptr)
	{
		//EC->SetEnemyType(ENEMYCHARACTER_MINION_RANGED);
		EC->SetEnemyType(ENEMYCHARACTER_MINION);
		EC->SetSpawnedRoom(nullptr);
		EC->SetObjectPoolManager(this);
		SpawnParticle(EC->GetSKMesh(), NAME_None, PARTICLE_MINION_SPAWN, FTransform());
	}
	ItemManager = GetGameInstance()->GetSubsystem<UCSubsystem_ItemManager>();
}

void ACGameState_Stage::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FString DebugMessage;
	int32 ActiveCount = 0;
	for (UPointLightComponent* PointLight : PointLights)
	{
		if (PointLight->IsActive()) ActiveCount++;
	}
	DebugMessage += FString::Printf(TEXT("PointLight Activated\t: %d / %d\n"), ActiveCount, PointLights.Num());
	int32 TotalCount = 0;
	int32 TotalActiveCount = 0;
	int32 idx = 0;
	for (TArray<UStaticMeshComponent*> StaticMeshComponentsarr : StaticMeshComponents)
	{
		ActiveCount = 0;
		for (UStaticMeshComponent* SM : StaticMeshComponentsarr)
		{
			if (SM->IsActive())
			{
				ActiveCount++;
				TotalActiveCount++;
			}
			TotalCount++;
		}
		DebugMessage += FString::Printf(TEXT("StaticMesh %d Activated\t: %d / %d\n"), idx, ActiveCount, StaticMeshComponentsarr.Num());
		//UE_LOG(LogTemp, Log, TEXT("StaticMesh %d Activated\t: %d / %d\n"), idx, ActiveCount, StaticMeshComponentsarr.Num());
		idx++;
	}
	DebugMessage += FString::Printf(TEXT("StaticMesh Activated\t: %d / %d\n"), TotalActiveCount, TotalCount);

	ActiveCount = 0;
	for (ACChest* Chest : Chests)
	{
		if (Chest->GetVisibility()) ActiveCount++;
	}
	DebugMessage += FString::Printf(TEXT("Chest Activated\t: %d / %d\n"), ActiveCount, Chests.Num());

	ActiveCount = 0;
	for (ACDoor* Door : Doors)
	{
		if (Door->GetVisibility()) ActiveCount++;
	}
	DebugMessage += FString::Printf(TEXT("Door Activated\t: %d / %d\n"), ActiveCount, Doors.Num());

	ActiveCount = 0;
	TotalCount = 0;
	for (TArray<AActor*> ItemArr : Items)
	{
		for (AActor* Item : ItemArr)
		{
			if (Item->GetRootComponent()->IsActive()) ActiveCount++;
			TotalCount++;
		}
	}
	DebugMessage += FString::Printf(TEXT("Items Activated\t: %d / %d\n"), ActiveCount, TotalCount);

	GEngine->AddOnScreenDebugMessage(0, DeltaSeconds, FColor::Green, *DebugMessage);
}

UPointLightComponent* ACGameState_Stage::GetPointLightComponent(class AActor* OwningActor)
{
	UPointLightComponent* rtn = nullptr;
	if (!PointLights_Available.IsEmpty()) PointLights_Available.Dequeue(rtn);
	else
	{
		rtn = NewObject<UPointLightComponent>(OwningActor);
		rtn->RegisterComponent();
		rtn->SetLightColor(FLinearColor(1.f, 0.914f, 0.601f));
		rtn->SetUseTemperature(true);
		rtn->SetTemperature(4800.f);
		//rtn->SetLightBrightness(6.f);
		if (rtn != nullptr)
		{
			PointLights.Add(rtn);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ACGameState_Stage : GetPointLightComponent : Fail To Create PointLightComponent"));
			return nullptr;
		}
	}
	if (rtn != nullptr)
	{
		rtn->Activate();
		rtn->SetVisibility(true);
	}
	return rtn;
}

void ACGameState_Stage::ReturnPointLightComponent(UPointLightComponent* PLC)
{
	PLC->SetVisibility(false);
	PLC->Deactivate();
	PointLights_Available.Enqueue(PLC);
}

UStaticMeshComponent* ACGameState_Stage::GetStaticMeshComponent(AActor* OwningActor, int32 Type, FTransform Transform)
{
	// TODO Candlalabra -> Add multiple staticmesh
	// Philar / Floor -> Valid Random Indexes
	if (Type == STAGE_GRID_CORRIDOR_FLOOR)
	{
		float temp = FMath::RandRange(0.f, 3.f);
		if (temp < 1.f) Type = STAGE_GRID_CORRIDOR_FLOOR1;
		else if (temp < 2.f) Type = STAGE_GRID_CORRIDOR_FLOOR2;
		else Type = STAGE_GRID_CORRIDOR_FLOOR3;
	}
	else if (Type == STAGE_GRID_CORRIDOR_PHILAR)
	{
		float temp = FMath::RandRange(0.f, 1.f);
		if (temp < 0.9f) Type = STAGE_GRID_CORRIDOR_PHILAR1;
		else Type = STAGE_GRID_CORRIDOR_PHILAR2;
	}

	if (!StaticMeshes.IsValidIndex(Type) || StaticMeshes[Type] == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ACGameState_Stage : GetStaticMeshComponent : Fail To Load UStaticMesh"));
		return nullptr;
	}
	if (!StaticMeshes_Available.IsValidIndex(Type))
	{
		UE_LOG(LogTemp, Error, TEXT("ACGameState_Stage : GetStaticMeshComponent : StaticMeshes_Available Index Out Of Range"));
		return nullptr;
	}
	UStaticMeshComponent* rtn = nullptr;
	if (!StaticMeshes_Available[Type].IsEmpty())
	{
		StaticMeshes_Available[Type].Dequeue(rtn);
	}
	else
	{
		rtn = NewObject<UStaticMeshComponent>(OwningActor);
		if (rtn != nullptr)
		{
			rtn->RegisterComponent();
			rtn->PrimaryComponentTick.SetTickFunctionEnable(false);
			rtn->SetStaticMesh(StaticMeshes[Type]);
			rtn->SetCanEverAffectNavigation(true);
			StaticMeshComponents[Type].Add(rtn);
		}
	}
	if (rtn != nullptr)
	{
		rtn->Activate();
		rtn->SetVisibility(true);
		rtn->SetHiddenInGame(false);
		rtn->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		rtn->SetCastShadow(true);
		rtn->AttachToComponent(OwningActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		rtn->SetRelativeTransform(Transform);
		rtn->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		rtn->SetCollisionResponseToAllChannels(ECR_Block);
		rtn->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
		//SetStaticMeshLOD(rtn, 2);
	}
	else UE_LOG(LogTemp, Error, TEXT("ACGameState_Stage : GetStaticMeshComponent : Fail To Create UStaticMeshComponent"));
	return rtn;
}

void ACGameState_Stage::ReturnStaticMeshComponent(UStaticMeshComponent* SMC, int32 Type)
{
	if (SMC == nullptr || !StaticMeshes_Available.IsValidIndex(Type)) return;
	SMC->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	SMC->Deactivate();
	SMC->SetVisibility(false);
	SMC->SetHiddenInGame(true);
	SMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SMC->SetCastShadow(false);
	StaticMeshes_Available[Type].Enqueue(SMC);
}

IIInteractableItem* ACGameState_Stage::GetChest(AActor* OwningActor, FTransform Transform)
{
	ACChest* rtn;
	if (Chests_Available.IsEmpty())
	{
		rtn = GetWorld()->SpawnActor<ACChest>(ACChest::StaticClass(), FTransform());
		Chests.Add(rtn);
	}
	else Chests_Available.Dequeue(rtn);

	if (rtn != nullptr)
	{
		rtn->AttachToActor(OwningActor, FAttachmentTransformRules::SnapToTargetIncludingScale);
		rtn->SetActorRelativeTransform(Transform);
		rtn->SetVisibility(true);
	}
	return rtn;
}

void ACGameState_Stage::ReturnChest(IIInteractableItem* Chest)
{
	ACChest* tempChest = Cast<ACChest>(Chest);
	if (tempChest != nullptr)
	{
		Chests_Available.Enqueue(tempChest);
		Chest->SetVisibility(false);
	}
}

IIInteractableItem* ACGameState_Stage::GetItem_InChest(AActor* OwningActor, int32 Type, FTransform Transform)
{
	return nullptr;
	//if (!ItemClasses.IsValidIndex(Type) || ItemClasses[Type] == nullptr) return nullptr;
	//if (!Item_Available.IsValidIndex(Type)) return nullptr;
	//AActor* rtn = nullptr;
	//IIInteractableItem* Irtn = nullptr;
	//if (Item_Available[Type].IsEmpty())
	//{
	//	rtn = GetWorld()->SpawnActor<AActor>(ItemClasses[Type], OwningActor->GetTransform() + Transform);
	//	if (rtn != nullptr)
	//	{
	//		Items[Type].Add(rtn);
	//		Irtn = Cast<IIInteractableItem>(rtn);
	//		Irtn->SetItemType(Type);
	//	}
	//}
	//else
	//{
	//	Item_Available[Type].Dequeue(rtn);
	//	Irtn = Cast<IIInteractableItem>(rtn);
	//}
	//if (Irtn != nullptr && GameModeDataManager != nullptr)
	//{
	//	Irtn->SetVisibility(true);
	//	Irtn->SetItemData(GameModeDataManager->GetItemDataAsset(Type));
	//}
	//return Irtn;
}

void ACGameState_Stage::ReturnItem(IIInteractableItem* Item, int32 Type)
{
	if (Item == nullptr) return;
	AActor* rtn = Cast<AActor>(Item);
	if (rtn == nullptr) return;
	Item_Available[Type].Enqueue(rtn);
	Item->SetVisibility(false);
}

void ACGameState_Stage::ReturnItem(AActor* Item, int32 Type)
{
	if (Item == nullptr) return;
	Item_Available[Type].Dequeue(Item);
	IIInteractableItem* temp = Cast<IIInteractableItem>(Item);
	if (temp != nullptr) temp->SetVisibility(false);
}

IIInteractableItem* ACGameState_Stage::GetDoor(AActor* OwningActor, FTransform Transform)
{
	ACDoor* rtn;
	if (Doors_Available.IsEmpty())
	{
		rtn = GetWorld()->SpawnActor<ACDoor>(ACDoor::StaticClass(), FTransform());
		Doors.Add(rtn);
	}
	else Doors_Available.Dequeue(rtn);

	if (rtn != nullptr)
	{
		rtn->AttachToActor(OwningActor, FAttachmentTransformRules::SnapToTargetIncludingScale);
		rtn->SetActorRelativeTransform(Transform);
		rtn->SetVisibility(true);
	}
	return rtn;
}

void ACGameState_Stage::ReturnDoor(IIInteractableItem* Door)
{
	ACDoor* tempDoor = Cast<ACDoor>(Door);
	if (tempDoor != nullptr)
	{
		tempDoor->SetVisibility(false);
		Doors_Available.Enqueue(tempDoor);
	}
}

IIInteractableItem* ACGameState_Stage::GetBrazier(AActor* OwningActor, FTransform Transform)
{
	ACBrazier* rtn;
	if (Braziers_Available.IsEmpty())
	{
		rtn = GetWorld()->SpawnActor<ACBrazier>(ACBrazier::StaticClass(), FTransform());
		Braziers.Add(rtn);
	}
	else Braziers_Available.Dequeue(rtn);

	if (rtn != nullptr)
	{
		rtn->AttachToActor(OwningActor, FAttachmentTransformRules::SnapToTargetIncludingScale);
		rtn->SetActorRelativeTransform(Transform);
		rtn->SetVisibility(true);
	}
	return rtn;
}

void ACGameState_Stage::ReturnBrazier(IIInteractableItem* Brazier)
{
	ACBrazier* tempBrazier = Cast<ACBrazier>(Brazier);
	if (tempBrazier != nullptr)
	{
		tempBrazier->SetVisibility(false);
		Braziers_Available.Enqueue(tempBrazier);
	}
}

IIEnemyCharacter* ACGameState_Stage::GetEnemyCharacter(AActor* OwningActor, int32 Type, FTransform Transform)
{
	if (!EnemyClasses.IsValidIndex(Type) || EnemyClasses[Type] == nullptr) return nullptr;
	if (!Enemy_Available.IsValidIndex(Type)) return nullptr;
	AActor* rtn = nullptr;
	IIEnemyCharacter* Irtn = nullptr;
	if (Enemy_Available[Type].IsEmpty())
	{
		Transform.SetLocation(Transform.GetLocation() + FVector(0.f, 0.f, 180.f));
		rtn = GetWorld()->SpawnActor<AActor>(EnemyClasses[Type], Transform);
		if (rtn != nullptr)
		{
			Enemies[Type].Add(rtn);
			Irtn = Cast<IIEnemyCharacter>(rtn);
			Irtn->SetEnemyType(Type);
		}
	}
	else
	{
		Enemy_Available[Type].Dequeue(rtn);
		Irtn = Cast<IIEnemyCharacter>(rtn);
		Transform.SetLocation(Transform.GetLocation() + FVector(0.f, 0.f, 180.f));
		rtn->SetActorTransform(Transform);
	}
	if (Irtn != nullptr)
	{
		Irtn->SetEnabled(true);
	}
	return Irtn;
}

void ACGameState_Stage::ReturnEnemyCharacter(IIEnemyCharacter* EnemyCharacter, int32 Type)
{
	if (EnemyCharacter == nullptr) return;
	AActor* rtn = Cast<AActor>(EnemyCharacter);
	if (rtn == nullptr) return;
	Enemy_Available[Type].Enqueue(rtn);
	EnemyCharacter->SetEnabled(false);
}

IIEquipment* ACGameState_Stage::GetEquipment(AActor* OwningActor, int32 Type, FTransform Transform)
{
	//return nullptr;
	if (!EquipmentsClasses.IsValidIndex(Type) || EquipmentsClasses[Type] == nullptr) return nullptr;
	if (!Equipments_Available.IsValidIndex(Type)) return nullptr;

	AActor* Equipment = nullptr;
	IIEquipment* rtn = nullptr;
	IIInteractableItem* Interactable = nullptr;
	if (Equipments_Available[Type].IsEmpty())
	{
		Equipment = GetWorld()->SpawnActor<AActor>(EquipmentsClasses[Type], Transform);
		if (Equipment != nullptr) Equipments[Type].Add(Equipment);
	}
	else Equipments_Available[Type].Dequeue(Equipment);
	rtn = Cast<IIEquipment>(Equipment);
	Interactable = Cast<IIInteractableItem>(Equipment);
	if (rtn != nullptr && Interactable != nullptr)
	{
		rtn->SetEqipmentType(Type);
		Interactable->SetVisibility(true);
		//Interactable->SetItemData(GameModeDataManager->GetEquipmentDataAsset(Type));
	}
	return rtn;
}

void ACGameState_Stage::ReturnEquipment(IIEquipment* Equipment, int32 Type)
{
	if (!Equipments_Available.IsValidIndex(Type)) return;
	AActor* temp = Cast<AActor>(Equipment);
	IIInteractableItem* Interactable = Cast<IIInteractableItem>(Equipment);
	if (temp != nullptr && Interactable != nullptr)
	{
		Equipments_Available[Type].Enqueue(temp);
		Interactable->SetVisibility(false);
		Equipment->Equip(false);
	}
}

UParticleSystemComponent* ACGameState_Stage::SpawnParticle(USceneComponent* AttachComponent, FName AttachPointName, int32 Type, FTransform Transform)
{
	if (ParticleSystems.IsValidIndex(Type))
	{
		if (AttachComponent != nullptr) return UGameplayStatics::SpawnEmitterAttached(
			ParticleSystems[Type],
			AttachComponent,
			AttachPointName,
			Transform.GetLocation(),
			Transform.GetRotation().Rotator(),
			EAttachLocation::SnapToTargetIncludingScale, true, EPSCPoolMethod::AutoRelease);
		else return UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystems[Type], Transform, true, EPSCPoolMethod::AutoRelease);
	}
	else return nullptr;
}

void ACGameState_Stage::SpawnProjectile(FTransform Transform, FDamageConfig DamageConfig, AActor* TargetActor, float Velocity, int32 ProjectileParticleType, USplineComponent* FollowTrace)
{
	ACProjectile* Projectile;
	if (!Projectiles_Available.IsEmpty()) Projectiles_Available.Dequeue(Projectile);
	else
	{
		Projectile = GetWorld()->SpawnActor<ACProjectile>(ACProjectile::StaticClass());
		Projectiles.Add(Projectile);
	}
	UParticleSystemComponent* tempParticle = SpawnParticle(Projectile->GetRootComponent(), NAME_None, ProjectileParticleType, FTransform());
	tempParticle->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	Projectile->SetParticleSystemComponent(tempParticle);
	Projectile->SetActorTransform(Transform);
	Projectile->SetObjectPoolManager(this);
	if (FollowTrace != nullptr) Projectile->Fire(DamageConfig, Velocity, FollowTrace);
	else Projectile->Fire(DamageConfig, (TargetActor->GetActorLocation() - Transform.GetLocation()).GetSafeNormal(), Velocity, 1200.f);
}

void ACGameState_Stage::SpawnProjectile(FTransform Transform, FDamageConfig DamageConfig, AActor* TargetActor, float Velocity, UParticleSystem* InSpawnParticle, USplineComponent* FollowTrace)
{
	ACProjectile* Projectile;
	if (!Projectiles_Available.IsEmpty()) Projectiles_Available.Dequeue(Projectile);
	else
	{
		Projectile = GetWorld()->SpawnActor<ACProjectile>(ACProjectile::StaticClass());
		Projectiles.Add(Projectile);
	}
	if (InSpawnParticle != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(
			InSpawnParticle,
			Projectile->GetRootComponent(),
			NAME_None,
			Transform.GetLocation(),
			Transform.GetRotation().Rotator(),
			EAttachLocation::SnapToTargetIncludingScale, true, EPSCPoolMethod::AutoRelease
		);
	}
	Projectile->SetActorTransform(Transform);
	Projectile->SetObjectPoolManager(this);
	if (FollowTrace != nullptr) Projectile->Fire(DamageConfig, Velocity, FollowTrace);
	else Projectile->Fire(DamageConfig, (TargetActor->GetActorLocation() - Transform.GetLocation()).GetSafeNormal(), Velocity, 1200.f);
}

void ACGameState_Stage::ReturnProjectile(ACProjectile* Projectile)
{
	Projectiles_Available.Enqueue(Projectile);
}

UCDamage* ACGameState_Stage::GetDamageComponent(AActor* OwningActor, FDamageConfig DamageConfig)
{
	UCDamage* DamageComponent = nullptr;
	if (!DamageComponents_Available.IsEmpty())
	{
		DamageComponents_Available.Dequeue(DamageComponent);
	}
	else
	{
		DamageComponent = NewObject<UCDamage>(OwningActor, UCDamage::StaticClass()); // Use NewObject for components
		if (DamageComponent != nullptr)
		{
			DamageComponents.Add(DamageComponent);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ACGameState_Stage : GetDamageComponent : Fail to create UCDamage"));
			return nullptr;
		}
	}

	if (DamageComponent != nullptr)
	{
		DamageComponent->RegisterComponent();
		DamageComponent->Activate();
		DamageComponent->SetDamageConfig(DamageConfig);
		DamageComponent->SetWorldLocationAndActivate((OwningActor->GetActorLocation() + DamageConfig.HitLocation) / 2.f);
		
	}
	return DamageComponent;
}

void ACGameState_Stage::ReturnDamageComponent(UCDamage* DamageComponent)
{
	if (DamageComponent != nullptr)
	{
		DamageComponent->Deactivate();
		//DamageComponent->SetVisibility(false);
		DamageComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform); // Detach from actor
		DamageComponents_Available.Enqueue(DamageComponent);
	}
}

void ACGameState_Stage::SetStaticMeshLOD(UStaticMeshComponent* StaticMeshComp, int32 LODs, bool IsNanite)
{
	if (StaticMeshComp == nullptr) return;

	StaticMeshComp->ForcedLodModel = LODs;
	if (IsNanite) StaticMeshComp->bForceDisableNanite = LODs > 0 ? 1 : 0;
	StaticMeshComp->MarkRenderStateDirty();

}

UCItemData* ACGameState_Stage::GetItemData(int32 ItemCategory, int32 ItemCode, int32 ItemCount)
{
	return ItemManager != nullptr ? ItemManager->GetOrCreateItemInstance(ItemCategory, ItemCode) : nullptr;
}

FVector ACGameState_Stage::GetRandomNavigatablePoint_ExclusiveRadius(FVector CurrentPosition, float MinDistance, float MaxDistance, FVector ExclusivePosition, float ExclusiveRadius, int32 Trial)
{
	if (NavSystem == nullptr) return CurrentPosition;
	FVector Dir_ToOrigin = (CurrentPosition - ExclusivePosition).GetSafeNormal2D();
	float SearchDistance = MinDistance;
	float perAngle = 120.f / Trial;
	FNavLocation ResultLocation;

	for (int32 Trying = 0; Trying < Trial; Trying++)
	{
		FRotator DirRotator = FRotator(0.f, perAngle * Trying * (Trying % 2 > 0 ? 1.f : -1.f), 0.f);
		FVector Dir_ToSearchPoint = DirRotator.RotateVector(Dir_ToOrigin);
		FVector SearchPoint = ExclusivePosition + Dir_ToSearchPoint * (ExclusiveRadius + SearchDistance);
		
		DrawDebugSphere(GetWorld(), SearchPoint, 50.f, 32, FColor::Cyan, false, 2.f, 0U, 1.f);
		if (NavSystem->GetRandomReachablePointInRadius(SearchPoint, SearchDistance, ResultLocation))
		{
			float DistFromOrigin = FVector::Dist(CurrentPosition, ResultLocation.Location);
			if (DistFromOrigin > MinDistance && DistFromOrigin < MaxDistance)
			{
				DrawDebugSphere(GetWorld(), ResultLocation.Location, 50.f, 32, FColor::Green, false, 2.f, 0U, 1.f);
				return ResultLocation.Location;
			}
		}
	}

	if (NavSystem->GetRandomReachablePointInRadius(ExclusivePosition + Dir_ToOrigin * ExclusiveRadius, SearchDistance, ResultLocation))
	{
		DrawDebugSphere(GetWorld(), ResultLocation.Location, 50.f, 32, FColor::Red, false, 2.f, 0U, 1.f);
		return ResultLocation.Location;
	}
	else return CurrentPosition;
}

bool ACGameState_Stage::GetRandomNagivatablePoint_AwayFromObject(FVector OirignPos, FVector AwayPos, float Distance, float Tolerance, FVector& OutPos)
{
	if (NavSystem == nullptr) return false;

	FVector Dir_ToOrigin = (OirignPos - AwayPos).GetSafeNormal2D();
	FVector SearchPos = AwayPos + Dir_ToOrigin * Distance;
	FNavLocation NavLocation;
	if (NavSystem->GetRandomReachablePointInRadius(SearchPos, Tolerance, NavLocation))
	{
		OutPos = NavLocation.Location;
		DrawDebugSphere(GetWorld(), NavLocation.Location, 50.f, 32, FColor::Green, false, 2.f, 0U, 1.f);
		return true;
	}
	else
	{
		SearchPos = AwayPos + FVector(Dir_ToOrigin.X, -Dir_ToOrigin.Y, Dir_ToOrigin.Z) * Distance;
		if (NavSystem->GetRandomReachablePointInRadius(SearchPos, Tolerance, NavLocation))
		{
			OutPos = NavLocation.Location;
			DrawDebugSphere(GetWorld(), NavLocation.Location, 50.f, 32, FColor::Green, false, 2.f, 0U, 1.f);
			return true;
		}
		SearchPos = AwayPos + FVector(-Dir_ToOrigin.X, Dir_ToOrigin.Y, Dir_ToOrigin.Z) * Distance;
		if (NavSystem->GetRandomReachablePointInRadius(SearchPos, Tolerance, NavLocation))
		{
			OutPos = NavLocation.Location;
			DrawDebugSphere(GetWorld(), NavLocation.Location, 50.f, 32, FColor::Green, false, 2.f, 0U, 1.f);
			return true;
		}
		//SearchPos = AwayPos + FVector(-Dir_ToOrigin.X, -Dir_ToOrigin.Y, Dir_ToOrigin.Z) * Distance;
		//if (NavSystem->GetRandomReachablePointInRadius(SearchPos, Tolerance, NavLocation))
		//{
		//	OutPos = NavLocation.Location;
		//	DrawDebugSphere(GetWorld(), NavLocation.Location, 50.f, 32, FColor::Green, false, 2.f, 0U, 1.f);
		//	return true;
		//}
	}
	return false;
}

void ACGameState_Stage::SetNavigationInvoker(AActor* Character)
{
	if (NavSystem == nullptr)
	{
		NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	}
	if (NavSystem != nullptr)
	{
		NavSystem->RegisterNavigationInvoker(Character);
	}
}

void ACGameState_Stage::RebuildNavMesh()
{
	if (NavSystem == nullptr)
	{
		NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	}
	if (NavSystem != nullptr)
	{
		//NavSystem->Build();
	}
}

void ACGameState_Stage::SetNavMeshLocation(FTransform& NewLocation)
{
	if (NavVolume == nullptr)
	{
		for (TActorIterator<ANavMeshBoundsVolume> It(GetWorld()); It; ++It)
		{
			ANavMeshBoundsVolume* tempVolume = *It;
			if (tempVolume != nullptr)
			{
				NavVolume = tempVolume;
				UBrushComponent* BC = NavVolume->GetBrushComponent();
				if (BC != nullptr) BC->SetMobility(EComponentMobility::Movable);
				break;
			}
		}
	}
	if (NavVolume != nullptr) NavVolume->SetActorTransform(NewLocation);
	if (NavSystem == nullptr) NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSystem != nullptr) NavSystem->OnNavigationBoundsUpdated(NavVolume);
}

void ACGameState_Stage::GenerateNextStage()
{
	ACGeneratedStage* GS = GetWorld()->SpawnActor<ACGeneratedStage>(ACGeneratedStage::StaticClass(), FVector(0.f, 0.f, Stages.Num() * -600.f), FRotator());
	GS->SetDelegate(StageClearedDelegatePtr);
	GS->SetStairCoord(StairCoord_x, StairCoord_y, StairCoord_d, StairCoord_x, StairCoord_y, StairCoord_d, Stages.IsEmpty());
	GS->SetCoord_Widght(43);
	GS->SetCoord_Height(43);
	GS->GenerateStage();
	Stages.Add(GS);
}

void ACGameState_Stage::GetTranslucentMaterialInstance(UStaticMesh* StaticMesh, TArray<class UMaterialInterface*>& OutArray)
{
	if (StaticMeshTranslucentMaterialInstance.Contains(StaticMesh)) OutArray = StaticMeshTranslucentMaterialInstance[StaticMesh];
}

void ACGameState_Stage::GetOpaqueMaterialInstance(UStaticMesh* StaticMesh, TArray<class UMaterialInterface*>& OutArray)
{
	if (StaticMeshOpaqueMaterialInstance.Contains(StaticMesh)) OutArray = StaticMeshOpaqueMaterialInstance[StaticMesh];
}
