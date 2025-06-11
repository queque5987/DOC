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
#include "Interfaces/IGameModeDataManager.h"

ACGameState_Stage::ACGameState_Stage() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponents.SetNum(	STAGE_GRID_CORRIDOR_NUM);
	StaticMeshes_Available.SetNum(	STAGE_GRID_CORRIDOR_NUM);
	StaticMeshes.SetNum(			STAGE_GRID_CORRIDOR_NUM);
	ItemClasses.SetNum(				INTERACTABLE_ITEM_NUM);
	Item_Available.SetNum(			INTERACTABLE_ITEM_NUM);
	Items.SetNum(					INTERACTABLE_ITEM_NUM);

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
	//if (Floor5Finder.Succeeded())				StaticMeshes[];
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

	ItemClasses[INTERACTABLE_ITEM_POTION_BLUE]		 = ACPotion::StaticClass();
	ItemClasses[INTERACTABLE_ITEM_POTION_GREEN]		 = ACPotion::StaticClass();
	ItemClasses[INTERACTABLE_ITEM_GEMSTONE_BLUE]	 = ACGemstone::StaticClass();
	ItemClasses[INTERACTABLE_ITEM_GEMSTONE_YELLOW]	 = ACGemstone::StaticClass();
	ItemClasses[INTERACTABLE_ITEM_GEMSTONE_RED]		 = ACGemstone::StaticClass();
	ItemClasses[INTERACTABLE_ITEM_GEMSTONE_GREEN]	 = ACGemstone::StaticClass();
	ItemClasses[INTERACTABLE_ITEM_GEMSTONE_PURPLE]	 = ACGemstone::StaticClass();
	ItemClasses[INTERACTABLE_ITEM_GEMSTONE_PINK]	 = ACGemstone::StaticClass();
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
	GenerateNextStage();
	GameModeDataManager = Cast<IIGameModeDataManager>(GetWorld()->GetAuthGameMode());
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
	ActiveCount = 0;
	int32 TotalCount = 0;
	for (TArray<UStaticMeshComponent*> StaticMeshComponentsarr : StaticMeshComponents)
	{
		for (UStaticMeshComponent* SM : StaticMeshComponentsarr)
		{
			if (SM->IsActive()) ActiveCount++;
			TotalCount++;
		}
	}
	DebugMessage += FString::Printf(TEXT("StaticMesh Activated\t: %d / %d\n"), ActiveCount, TotalCount);

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
	//int i = 0;
	//for (const UPointLightComponent* PointLight : PointLights)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("%d \t %s \t %s"), i, *PointLight->GetName(), (PointLight->IsActive() ? TEXT("Active") : TEXT("DeActive")));
	//	i++;
	//}
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
	if (!StaticMeshes_Available[Type].IsEmpty()) StaticMeshes_Available[Type].Dequeue(rtn);
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
	if (!ItemClasses.IsValidIndex(Type) || ItemClasses[Type] == nullptr) return nullptr;
	if (!Item_Available.IsValidIndex(Type)) return nullptr;
	AActor* rtn = nullptr;
	IIInteractableItem* Irtn = nullptr;
	if (Item_Available[Type].IsEmpty())
	{
		rtn = GetWorld()->SpawnActor<AActor>(ItemClasses[Type], OwningActor->GetTransform() + Transform);
		if (rtn != nullptr)
		{
			Items[Type].Add(rtn);
			Irtn = Cast<IIInteractableItem>(rtn);
			Irtn->SetItemType(Type);
		}
	}
	else
	{
		Item_Available[Type].Dequeue(rtn);
		Irtn = Cast<IIInteractableItem>(rtn);
	}
	if (Irtn != nullptr && GameModeDataManager != nullptr)
	{
		Irtn->SetVisibility(true);
		Irtn->SetItemData(GameModeDataManager->GetItemDataAsset(Type));
	}
	return Irtn;
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

void ACGameState_Stage::RebuildNavMesh()
{
	if (NavSystem == nullptr)
	{
		NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	}
	if (NavSystem != nullptr)
	{
		NavSystem->Build();
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
	GS->SetStairCoord(StairCoord_x, StairCoord_y, StairCoord_d, StairCoord_x, StairCoord_y, StairCoord_d, Stages.IsEmpty());
	GS->SetCoord_Widght(40);
	GS->SetCoord_Height(40);
	GS->GenerateStage();
	Stages.Add(GS);
}
