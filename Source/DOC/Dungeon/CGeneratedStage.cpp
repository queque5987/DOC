#include "CGeneratedStage.h"
#include "Dungeon/CStageGrid_Corridor.h"
#include "Dungeon/CGeneratedRoom.h"
#include "Interfaces/IStageGrid_Meshes.h"
#include "Interfaces/IGameStateLightManager.h"
#include "Dungeon/CGeneratedStair.h"
#include "Engine/StaticMeshActor.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameState.h"
#include "GameFramework/Character.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Components/TextRenderComponent.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerControllerUI.h"
#include "Interfaces/IHUD.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IStageGrid_Room.h"
#include "Dungeon/Enemies/Minion/CMinion.h"
#include "DrawDebugHelpers.h"

const int32 ACGeneratedStage::ROOM_BLANK			= 0;
const int32 ACGeneratedStage::ROOM_OCCUPIED			= 1;
const int32 ACGeneratedStage::ROOM_CORRIDOR			= 2;
const int32 ACGeneratedStage::ROOM_STAIR			= 3;
const int32 ACGeneratedStage::ROOM_STAIR_DOOR		= 4;
const int32 ACGeneratedStage::ROOM_BOSS				= 5;
const int32 ACGeneratedStage::ROOM_BOSS_DOOR		= 6;
const int32 ACGeneratedStage::ROOM_EDGE				= 7;
//const int32 ACGeneratedStage::ROOM_EDGE_LOCK		= 8;
const int32 ACGeneratedStage::ROOM_DEBUG_BLOCK		= 9;

ACGeneratedStage::ACGeneratedStage() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UTexture> MinimapTileFinder(TEXT("/Game/UI/Blank.Blank"));
	if (MinimapTileFinder.Succeeded()) MinimapTile = MinimapTileFinder.Object;
	ConstructorHelpers::FObjectFinder<UTexture> MinimapTile_BlueFinder(TEXT("/Game/UI/Blue.Blue"));
	if (MinimapTile_BlueFinder.Succeeded()) MinimapTile_Blue = MinimapTile_BlueFinder.Object;
	ConstructorHelpers::FObjectFinder<UTexture> MinimapTile_YellowFinder(TEXT("/Game/UI/Yellow.Yellow"));
	if (MinimapTile_YellowFinder.Succeeded()) MinimapTile_Yellow = MinimapTile_YellowFinder.Object;

	Coord_Width = 35;
	Coord_Height = 35;
	Room_Ratio = 0.02f;
	Max_Corridor_Length = 5;
	Room_Distance = 5.f;
	Clamp_Ratio = 0.6f;
	Clamp_Criteria_Ratio = 0.1f;
	Max_Clamp_Level = 3;
	Stage_Grid_Meshes.SetNum(STAGE_GRID_MESH_TYPES);

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
}

void ACGeneratedStage::SetStairCoord(int32 Coord_H, int32 Coord_W, int32 Direction, int32& rtn_Coord_H, int32& rtn_Coord_W, int32& rtn_Direction, bool bIsEntrance)
{
	StairFromTopCoordinate.Get<0>() = Coord_H;
	StairFromTopCoordinate.Get<1>() = Coord_W;
	StairFromTopCoordinate.Get<2>() = Direction;
	
	int32 a = FMath::FloorToInt32(FMath::RandRange(2.f, Coord_Height - 6.f));
	int32 b = FMath::FloorToInt32(FMath::RandRange(2.f, Coord_Width - 6.f));
	int32 d = FMath::FloorToInt32(FMath::RandRange(0.f, 4.f));

	if (Coord_H <= a + 5 && a - 1 <= Coord_H + 4)
	{
		a += 6;
		a %= (Coord_Height - 6);
		if (a < 2) a++;
	}
	if (Coord_W <= b + 5 && b - 1 <= Coord_W + 4)
	{
		b += 6;
		b %= (Coord_Width - 6);
		if (b < 2) b++;
	}

	StairToBottomCoordinate.Get<0>() = a;
	StairToBottomCoordinate.Get<1>() = b;
	StairToBottomCoordinate.Get<2>() = d;

	rtn_Coord_H = StairToBottomCoordinate.Get<0>();
	rtn_Coord_W = StairToBottomCoordinate.Get<1>();
	rtn_Direction = StairToBottomCoordinate.Get<2>();
	
	float Adjust_x = 0.f;
	float Adjust_y = 0.f;
	if (Direction == 0) Adjust_x = 1200.f;
	else if (Direction == 1) Adjust_y = 0.f;
	else if (Direction == 2)
	{
		Adjust_y = 1200.f;
	}
	else if (Direction == 3)
	{
		Adjust_x = 1200.f;
		Adjust_y = 1200.f;
	}
	CurrentPlayerCoordinate = StairFromTopCoordinate;
	ACGeneratedStair* Grid_Stair = GetWorld()->SpawnActor<ACGeneratedStair>(ACGeneratedStair::StaticClass(),
		FTransform(
			FRotator(0.f, 90.f - 90.f * Direction, 0.f),
			GetActorLocation() + FVector(
				StairFromTopCoordinate.Get<0>() * 400.f + Adjust_x,
				StairFromTopCoordinate.Get<1>() * 400.f + Adjust_y,
				0.f)
		)
	);
	if (Grid_Stair != nullptr)
	{
		Grid_Stair->GenerateStair(bIsEntrance, &Delegate_LightOn, &Delegate_Entered_Stage);
		Stage_Grid_Meshes[STAGE_GRID_MESH_STAIR] = Grid_Stair;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ACGeneratedStage : SetStairCoord : Fail to Create Stair"));
	}
}

//void ACGeneratedStage::SetMinimapRenderTarget(UCanvasRenderTarget2D* T)
//{
//	MinimapRenderTarget = T;
//	if (MinimapRenderTarget != nullptr)
//	{
//		MinimapRenderTarget->OnCanvasRenderTargetUpdate.AddDynamic(this, &ACGeneratedStage::UpdateMinimap);
//	}
//}


void ACGeneratedStage::BeginPlay()
{
	Super::BeginPlay();

	AActor* Grid_Floor = GetWorld()->SpawnActor<ACStageGrid_Corridor>(ACStageGrid_Corridor::StaticClass(), GetActorTransform());
	Grid_Floor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	Stage_Grid_Meshes[STAGE_GRID_MESH_CORRIDOR] = Grid_Floor;
	if (GetWorld() != nullptr && GetWorld()->GetGameState() != nullptr)
	{
		LightManager = Cast<IIGameStateLightManager>(GetWorld()->GetGameState());
		ObjectPoolManager = Cast<IIObjectPoolManager>(GetWorld()->GetGameState());
	}
	Delegate_LightOn.BindUFunction(this, TEXT("LightsOn"));
	Delegate_Entered_Stage.BindUFunction(this, TEXT("Entered_Stage"));
}

void ACGeneratedStage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MinimapRenderTarget != nullptr)
	{
		MinimapRenderTarget->UpdateResource();
	}
	if (WidemapRenderTarget != nullptr)
	{
		WidemapRenderTarget->UpdateResource();
	}

	if (IPlayerCharacter_Stage != nullptr)
	{
		FVector CurrentRelativeLocation = IPlayerCharacter_Stage->GetLocation() - GetActorLocation();
		FCoordinate PlayerCoord{
			FMath::FloorToInt32((CurrentRelativeLocation.X + 200.f) / 400.f),
			FMath::FloorToInt32((CurrentRelativeLocation.Y + 200.f) / 400.f),
			0
		};
		if (CurrentPlayerCoordinate != PlayerCoord)
		{
			Stage_CalculateDistance(PlayerCoord, true);
			CurrentPlayerCoordinate = PlayerCoord;
		}
	}
}

void ACGeneratedStage::GenerateStage()
{
	Stage_Room_Coord.SetNum(Coord_Height);
	for (int32 h = 0; h < Coord_Height; h++)
	{
		for (int32 w = 0; w < Coord_Width; w++)
		{
			Stage_Room_Coord[h].Emplace(FCoordinate(h, w, 0), -1, ROOM_BLANK, -1);
		}
	}

	auto RoomCapacity = (int32)(Coord_Width * Coord_Height * Room_Ratio);

	/*
		Create Stair Rooms
	*/

	int32 SFTCx = StairFromTopCoordinate.Get<0>();
	int32 SFTCy = StairFromTopCoordinate.Get<1>();
	int32 SFTCd = StairFromTopCoordinate.Get<2>();
	for (int32 i = 0; i < 4; i++)
	{
		for (int32 j = 0; j < 4; j++)
		{
			int32 x = SFTCx + i;
			int32 y = SFTCy + j;
			int32 d = SFTCd;
			if (
				((d == 2 && (i == 0 && j == 3)) || (d == 3 && (i == 3 && j == 3))) ||
				((d == 1 && (i == 0 && j == 0)) || (d == 0 && (i == 3 && j == 0)))
				)
			{
				Stage_Room_Coord[x][y].State = ROOM_STAIR_DOOR;
				Stage_Room_Coord[x][y].Index = Stage_Rooms.Num();
				Stage_Room_Coord[x][y].Generated_Room = Cast<IIStageGrid_Room>(Stage_Grid_Meshes[STAGE_GRID_MESH_STAIR]);
				Stage_Room_Coord[x][y].Root = Stage_Rooms.Num();
				Stage_Rooms.Add(&Stage_Room_Coord[x][y]);
				Stage_Room_Coord[x - Directions[d].Get<0>()][y - Directions[d].Get<1>()].State = ROOM_STAIR;
				StartDoorCoordinate.Get<0>() = x;
				StartDoorCoordinate.Get<1>() = y;
			}
			else Stage_Room_Coord[x][y].State = ROOM_STAIR;
			Stage_Room_Coord[x][y].ShowOnMinimap = true;
		}
	}
	int32 STBCx = StairToBottomCoordinate.Get<0>();
	int32 STBCy = StairToBottomCoordinate.Get<1>();
	int32 STBCd = StairToBottomCoordinate.Get<2>();
	for (int32 i = 0; i < 4; i++)
	{
		for (int32 j = 0; j < 4; j++)
		{
			int32 x = STBCx + i;
			int32 y = STBCy + j;
			int32 d = STBCd;
			if (
				((d == 2 && (i == 0 && j == 0)) || (d == 3 && (i == 0 && j == 3))) ||
				((d == 1 && (i == 3 && j == 0)) || (d == 0 && (i == 3 && j == 3)))
				)
			{
				Stage_Room_Coord[x][y].State = ROOM_STAIR_DOOR;
				Stage_Room_Coord[x][y].Index = Stage_Rooms.Num();
				Stage_Room_Coord[x][y].Root = Stage_Rooms.Num();
				Stage_Rooms.Add(&Stage_Room_Coord[x][y]);
				Stage_Room_Coord[x + Directions[d].Get<0>()][y + Directions[d].Get<1>()].State = ROOM_STAIR;
			}
			else Stage_Room_Coord[x][y].State = ROOM_STAIR;
		}
	}

	/*
		Generate Rooms
	*/
	for (int32 i = 0; i < RoomCapacity; i++)
	{
		for (int32 j = 0; j < 3; j++) // Try 3 Times
		{
			double H = FMath::FRandRange(0.f, 1.f);
			double W = FMath::FRandRange(0.f, 1.f);
			int32 h = FMath::FloorToInt32(H * Coord_Height);
			int32 w = FMath::FloorToInt32(W * Coord_Width);

			bool bBoundaryCheck = true;
			for (int32 k = h - Room_Distance; k < h + Room_Distance; k++)
			{
				for (int32 l = w - Room_Distance; l < w + Room_Distance; l++)
				{
					if ((k < 0 || l < 0) || (k >= Coord_Height) || (l >= Coord_Width)) continue;
					if (FMath::Square(k - h) + FMath::Square(l - w) > FMath::Square(Room_Distance)) continue;
					if (Stage_Room_Coord[k][l].State != ROOM_BLANK)
					{
						bBoundaryCheck = false;
						break;
					}
				}
			}
			if ((h < 0 || w < 0) || (h >= Coord_Height) || (w >= Coord_Width)) continue;
			if (bBoundaryCheck && Stage_Room_Coord[h][w].State == ROOM_BLANK)
			{
				Stage_Room_Coord[h][w].State = ROOM_OCCUPIED;
				Stage_Room_Coord[h][w].Index = Stage_Rooms.Num();
				Stage_Room_Coord[h][w].Root = Stage_Rooms.Num();
				Stage_Rooms.Add(&Stage_Room_Coord[h][w]);
				break;
			}
		}
	}

	RoomCapacity = Stage_Rooms.Num(); // Rooms Actually Created

	///*
	//	Primary Union
	//*/
	//for (int i = 0; i < RoomCapacity; i++)
	//{
	//	for (int j = i + 1; j < RoomCapacity; j++)
	//	{
	//		FCoordinate iCoord = Stage_Rooms[i]->Coordinate;
	//		FCoordinate jCoord = Stage_Rooms[j]->Coordinate;

	//		if ((iCoord.Get<0>() == jCoord.Get<0>() && FMath::Abs(iCoord.Get<1>() - jCoord.Get<1>()) <= 1) ||
	//			(iCoord.Get<1>() == jCoord.Get<1>() && FMath::Abs(iCoord.Get<0>() - jCoord.Get<0>()) <= 1))
	//		{
	//			UE_LOG(LogTemp, Log, TEXT("Before Union : %d, %d"), Room_Find(i), Room_Find(j));
	//			Room_Union(i, j);
	//			UE_LOG(LogTemp, Log, TEXT("After Union : %d, %d"), Room_Find(i), Room_Find(j));
	//			break;
	//		}
	//	}
	//}

	///*
	//	Shuffle
	//*/
	//for (int32 i = Stage_Rooms.Num() - 1; i > 0; i--)
	//{
	//	auto j = (int32)FMath::RandRange(0, i);
	//	if (i != j) Stage_Rooms.Swap(i, j);
	//}

	/*
		Create Boss Room
	*/

	int32 idx = FMath::RandRange(0, RoomCapacity);

	for (int r = 0; r < RoomCapacity; r++)
	{
		FRoom_Info* room = Stage_Rooms[(idx + r) % RoomCapacity];
		int32 i = room->Coordinate.Get<0>();
		int32 j = room->Coordinate.Get<1>();
		if (bCreateBossRoom)
		{
			bool bQualify = true;
			int32 R = FMath::FloorToInt32(FMath::RandRange(0.f, 4.f));
			for (int32 l = 0; l < 4; l++)
			{
				// Depth Check
				for (int32 depth = 1; depth < 5; depth++)
				{
					for (int32 rng = 0; rng < 4; rng++)
					{
						if (depth > 1 && rng == 2) continue;
						else if (depth > 2 && (rng == 1 || rng == 3)) continue;
						FCoordinate dir = Directions[(R + rng) % 4];
						int32 x = dir.Get<0>() * depth + i;
						int32 y = dir.Get<1>() * depth + j;
						if ((x < 0 || y < 0) || (x >= Coord_Height) || (y >= Coord_Width))
						{
							bQualify = false;
							break;
						}
						if (Stage_Room_Coord[x][y].State != ROOM_BLANK)
						{
							bQualify = false;
							break;
						}
					}
					if (!bQualify) break;
				}
				if (bQualify) break;
				R++;
				R %= 4;
			}
			if (bQualify)
			{
				for (int32 ydepth = -2; ydepth < 3; ydepth++)
				{
					FCoordinate dir = Directions[(R + (ydepth < 0 ? 1 : 3)) % 4];
					int32 x = i + dir.Get<0>() * FMath::Abs(ydepth);
					int32 y = j + dir.Get<1>() * FMath::Abs(ydepth);
					for (int32 xdepth = 0; xdepth < 5; xdepth++)
					{
						FCoordinate depth_dir = Directions[R];
						int32 depth_x = x + depth_dir.Get<0>() * xdepth;
						int32 depth_y = y + depth_dir.Get<1>() * xdepth;

						Stage_Room_Coord[depth_x][depth_y].State = ((ydepth == 0 && xdepth == 0) ? ROOM_BOSS_DOOR : ROOM_BOSS);
						Stage_Room_Coord[depth_x][depth_y].Index = room->Root;
						Stage_Room_Coord[depth_x][depth_y].Root = room->Root;
						if (ydepth == 0 && xdepth == 2)
						{
							BossRoomCenterCoordinate = FCoordinate{ depth_x , depth_y, 0 };
							if (room->Generated_Room != nullptr) room->Generated_Room->SetRoomRelativeLocation(FVector(depth_x * 400.f, depth_y * 400.f, 0.f));
						}
						if (room->Generated_Room == nullptr)
						{
							ACGeneratedRoom* tempRoom = GetWorld()->SpawnActor<ACGeneratedRoom>(ACGeneratedRoom::StaticClass(),
								FTransform(
									FRotator(0.f, 0.f, 0.f), GetActorLocation()
								)
							);
							if (tempRoom != nullptr)
							{
								tempRoom->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
								room->Generated_Room = tempRoom;
								Stage_Room_Coord[depth_x][depth_y].Generated_Room = room->Generated_Room;
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("Fail To Create Boss Room"));
							}
						}
						else
						{
							Stage_Room_Coord[depth_x][depth_y].Generated_Room = room->Generated_Room;
						}
					}
				}
				bCreateBossRoom = false;
				break;
			}
			if (!bCreateBossRoom) continue;
		}
	}

	/*
		Connect Rooms
	*/
	for (int i = 0; i < RoomCapacity; i++)
	{
		TArray<FCoordinate> Trail;
		Trail.Reserve(Max_Corridor_Length + 1);
		Trail.Add(Stage_Rooms[i]->Coordinate);
		if (Stage_CalculateClosestCorridor(Trail))
		{
			FCoordinate ConnectedNodeCoord;
			//FRoom_Info* PrevRoom = nullptr;
			for (const FCoordinate& t : Trail)
			{
				FRoom_Info& TravRoom = Stage_Room_Coord[t.Get<0>()][t.Get<1>()];
				if (TravRoom.State != ROOM_OCCUPIED && TravRoom.State != ROOM_BOSS_DOOR)
				{
					if (TravRoom.State == ROOM_BLANK)
					{
						TravRoom.State = ROOM_CORRIDOR;
						TravRoom.Root = i;	// Set Corridor's Root As Clostest Room
					}
					else if (TravRoom.State != ROOM_BOSS)
					{
						Room_Union(i, TravRoom.Root);
					}
				}
				else
				{
					Room_Union(i, Room_Find(TravRoom.Root));
				}
			}
			if (Room_FindClosestNode(Stage_Rooms[i]->Coordinate, ConnectedNodeCoord))
			{
				for (int j = 0; j < RoomCapacity; j++)
				{
					if (Stage_Rooms[j]->Coordinate == Trail[0])
					{
						Room_Union(i, j);
						//break;
					}
				}
			}
			else UE_LOG(LogTemp, Error, TEXT("CGameMode_Stage : GenerateStage : Could not Find Closest Node Connected"));
		}
	}

	/*
		Check Union
	*/
	TSet<int32> SRoots;
	for (int32 i = 0; i < Stage_Rooms.Num(); i++)
	{
		SRoots.Add(Room_Find(i));
	}
	UE_LOG(LogTemp, Log, TEXT("CGameMode_Stage : GenerateStage : %d"), SRoots.Num());

	/*
		Set Edge Rooms
	*/
	bool bCreatedBossRoom = false;
	int32 i_m = 0;
	int32 i_M = 0;
	int32 j_m = 0;
	int32 j_M = 0;
	for (FRoom_Info* room : Stage_Rooms)
	{
		int32 sum = 0;
		if (room->State == ROOM_BOSS) continue;

		int32 x = room->Coordinate.Get<0>();
		int32 y = room->Coordinate.Get<1>();
		i_m = (x == 0 ? x : (x < Coord_Height - 1 ? x - 1 : x - 2));
		i_M = (x == 0 ? x + 3 : (x < Coord_Height - 1 ? x + 2 : x + 1));
		j_m = (y == 0 ? y : (y < Coord_Width - 1 ? y - 1 : y - 2));
		j_M = (y == 0 ? y + 3 : (y < Coord_Width - 1 ? y + 2 : y + 1));

		int32 ConnectedCounter = 0;
		for (int i = i_m; i < i_M; i++)
		{
			for (int j = j_m; j < j_M; j++)
			{
				if ((i < 0 || j < 0) || (i >= Coord_Height || j >= Coord_Width)) continue;
				if (i == x && j == y) continue;
				int32 ManhDistFromRoom = FMath::Abs(i - x) + FMath::Abs(j - y);
				if ((ManhDistFromRoom == 1 && sum <= 1) && Stage_Room_Coord[i][j].State == ROOM_CORRIDOR)
				{
					for (const FCoordinate& dir : Directions)
					{
						int32 _x = dir.Get<0>();
						int32 _y = dir.Get<1>();
						if ((i + _x < 0 || j + _y < 0) || (i + _x >= Coord_Height || j + _y >= Coord_Width)) continue;
						if (Stage_Room_Coord[i + _x][j + _y].State == ROOM_CORRIDOR) ConnectedCounter++;
					}
				}
				if (Stage_Room_Coord[i][j].State != ROOM_BLANK) sum++;
			}
		}

		if (sum == 1 || (sum == 2 && ConnectedCounter == 1))
		{
			//int32 x = room->Coordinate.Get<0>();
			//int32 y = room->Coordinate.Get<1>();
			//i_m = (x == 0 ? x : (x < Coord_Height - 1 ? x - 1 : x - 2));
			//i_M = (x == 0 ? x + 3 : (x < Coord_Height - 1 ? x + 2 : x + 1));
			//j_m = (y == 0 ? y : (y < Coord_Width - 1 ? y - 1 : y - 2));
			//j_M = (y == 0 ? y + 3 : (y < Coord_Width - 1 ? y + 2 : y + 1));

			for (int i = i_m; i < i_M; i++)
			{
				for (int j = j_m; j < j_M; j++)
				{
					if ((i < 0 || j < 0) || (i >= Coord_Height || j >= Coord_Width)) continue;
					Stage_Room_Coord[i][j].State = ROOM_EDGE;
					Stage_Room_Coord[i][j].Floor = room->Floor;
					if (room->Generated_Room == nullptr)
					{
						ACGeneratedRoom* tempRoom = GetWorld()->SpawnActor<ACGeneratedRoom>(ACGeneratedRoom::StaticClass(),
							FTransform(
								FRotator(0.f, 0.f, 0.f), GetActorLocation()
							)
						);
						if (tempRoom == nullptr)
						{
							UE_LOG(LogTemp, Error, TEXT("Fail To Create Generated Room"));
						}
						else
						{
							tempRoom->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
							room->Generated_Room = tempRoom;
							Stage_Room_Coord[i][j].Generated_Room = room->Generated_Room;
						}
					}
					else
					{
						Stage_Room_Coord[i][j].Generated_Room = room->Generated_Room;
					}
				}
			}
			if (room->Generated_Room != nullptr) room->Generated_Room->SetRoomRelativeLocation(FVector((i_m + 1) * 400.f, (j_m + 1) * 400.f, 0.f));
		}
	}

	/*
		Calculate Distance
	*/
	Stage_CalculateDistance(StartDoorCoordinate);

	//for (FRoom_Info* room : Stage_Rooms)
	//{
	//	if (room->State != ROOM_OCCUPIED) continue;
	//}

	/*
		Spawn Rooms
		Deprecated - Using HISM
	
	
	for (int i = 0; i < Coord_Height; i++)
	{
		for (int j = 0; j < Coord_Width; j++)
		{
			if (Stage_Room_Coord[i][j].State == ROOM_BLANK) continue;
			else if (Stage_Room_Coord[i][j].State == ROOM_CORRIDOR || Stage_Room_Coord[i][j].State == ROOM_OCCUPIED)
			{
				// Debug

				UTextRenderComponent* TextRender = NewObject<UTextRenderComponent>(this);
				TextRender->RegisterComponent();
				//TextRender->AttachToComponent(SMC, FAttachmentTransformRules::SnapToTargetIncludingScale);
				TextRender->SetRelativeScale3D(FVector(4.f, 4.f, 4.f));
				TextRender->SetRelativeRotation(FRotator(90.f, 0.f, 180.f));
				TextRender->SetRelativeLocation(GetActorLocation() + (FVector(i * 100.f * Stage_Scale, j * 100.f * Stage_Scale, 0.f) + FVector(50.f, 50.f, 110.f)));
				TextRender->SetText(FText::FromString(FString::FromInt(Stage_Room_Coord[i][j].DistFromEntrance)));
				Stage_Room_Coord[i][j].DebugTextComponent = TextRender;

				// Floor
				GridAddMesh(
					FTransform(
						FVector(i * 100.f * Stage_Scale, j * 100.f * Stage_Scale, 0.f)
					),
					STAGE_GRID_MESH_CORRIDOR,
					STAGE_GRID_CORRIDOR_FLOOR
				);
				// Ceiling
				GridAddMesh(
					FTransform(
						FRotator(0.f, 0.f, 180.f),
						FVector(i * 100.f * Stage_Scale, j * 100.f * Stage_Scale, 500.f)
					),
					STAGE_GRID_MESH_CORRIDOR,
					STAGE_GRID_CORRIDOR_FLOOR
				);
				int d = 0;
				for (const FCoordinate& dir : Directions)
				{
					FCoordinate tempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, dir);

						// Candle
						// Create Candle Every 6 Grid

					// Can Not Be Placed Because There's No Wall
					bool bCreateCandleReverse = false;
					if (Stage_Room_Coord[i][j].DistFromEntrance > 0 &&
						(Stage_Room_Coord[i][j].DistFromEntrance - 1) % 2 == 0)
					{
						FCoordinate way0coord = SumCoordinate(
							Stage_Room_Coord[i][j].Coordinate, Directions[((Stage_Room_Coord[i][j].DistFromEntrance - 1) % 4 == 0) ? 0 : 2]
							);
						FCoordinate way1coord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[((Stage_Room_Coord[i][j].DistFromEntrance - 1) % 4 == 0) ? 1 : 3]);
						bool CreateAt0 = false;
						bool CreateAt1 = false;
						if ((!IsCoordinateInBound(way0coord) || (
							(
								(
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].State == ROOM_BLANK ||
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].State == ROOM_BOSS
									) ||
								(
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].State == ROOM_STAIR
									)
								) ||
							(
								Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].State == ROOM_EDGE && (
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].Generated_Room != nullptr &&
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].Generated_Room->GetHasDoor()
									)
								)
							)))
						{
							CreateAt0 = true;
						}
						if ((!IsCoordinateInBound(way1coord) || (
							(
								(
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].State == ROOM_BLANK ||
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].State == ROOM_BOSS
									) ||
								(
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].State == ROOM_STAIR
									)
								) ||
							(
								Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].State == ROOM_EDGE && (
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].Generated_Room != nullptr &&
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].Generated_Room->GetHasDoor()
									)
								)
							)))
						{
							CreateAt1 = true;
						}
						if (!CreateAt0 && !CreateAt1)
						{
							bCreateCandleReverse = true;
						}

						if (((Stage_Room_Coord[i][j].DistFromEntrance - 1) % 4 == 0 && ((!bCreateCandleReverse && (d == 0 || d == 1)) || (bCreateCandleReverse && (d == 3 || d == 2)))) ||
							(Stage_Room_Coord[i][j].DistFromEntrance - 1) % 4 != 0 && ((!bCreateCandleReverse && (d == 3 || d == 2)) || (bCreateCandleReverse && (d == 0 || d == 1))))
						{
							if (!IsCoordinateInBound(tempCoord) || (
								(
									(
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BLANK ||
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS
										) ||
									(
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_STAIR
										)
									) ||
								(
									Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE && (
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room != nullptr &&
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room->GetHasDoor()
										)
									)
								))
							{
								// Spawn Candlelabra
								GridAddMesh(
									FTransform(
										FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											180.f)
									),
									STAGE_GRID_MESH_CORRIDOR,
									STAGE_GRID_CORRIDOR_CANDLELABRA
								);
								Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 72.f,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 72.f,
									180.f + 66.f));
								Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 54.f,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 54.f,
									180.f + 127.f));
								Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
								Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
								//PLC->SetLightColor(FLinearColor(1.f, 0.73f, 0.5f));
							}
						}
					}

					// Spawn Wall
					if (!IsCoordinateInBound(tempCoord) || (
						(
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BLANK ||
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS
								) ||
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_STAIR
								)
							) ||
						(
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE && (
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room != nullptr &&
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room->GetHasDoor()
								)
							)
						))
					{
						//Wall Base Bottom
						GridAddMesh(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									0.f)
							),
							STAGE_GRID_MESH_CORRIDOR,
							STAGE_GRID_CORRIDOR_WALL_BASE
						);
						// Wall
						GridAddMesh(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									0.f)
							),
							STAGE_GRID_MESH_CORRIDOR,
							STAGE_GRID_CORRIDOR_WALL
						);
						// Wall Base Top
						GridAddMesh(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									240.f)
							),
							STAGE_GRID_MESH_CORRIDOR,
							STAGE_GRID_CORRIDOR_WALL_BASE
						);

						// Philar Edge * 2
						if (d == 3)
						{
							GridAddMesh(
								FTransform(
									FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
									FVector(
										i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
										0.f)
								),
								STAGE_GRID_MESH_CORRIDOR,
								STAGE_GRID_CORRIDOR_PHILAR
							);
							GridAddMesh(
								FTransform(
									FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
									FVector(
										i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
										0.f)
								),
								STAGE_GRID_MESH_CORRIDOR,
								STAGE_GRID_CORRIDOR_PHILAR
							);
						}
						// Wall Arch * 4

						GridAddMesh(
							FTransform(
								FRotator(0.f, -90.f + 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									300.f)
							),
							STAGE_GRID_MESH_CORRIDOR,
							STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
						);
					} // Spawn Door Not Wall
					else if (IsCoordinateInBound(tempCoord) && (
						(
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_OCCUPIED ||
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_STAIR_DOOR
							) || (
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE ||
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS_DOOR
							)
						)
						)
					{
						bool bIsBossRoom = Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS ? true : false;
						if (Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE &&
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room != nullptr)
						{
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room->SetHasDoor(true);
						}
						//Wall Door Base Bottom
						GridAddMesh(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									0.f)
							),
							STAGE_GRID_MESH_CORRIDOR,
							STAGE_GRID_CORRIDOR_WALL_DOOR_BASE
						);
						// Wall Door
						GridAddMesh(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									0.f
								)
							),
							STAGE_GRID_MESH_CORRIDOR,
							STAGE_GRID_CORRIDOR_WALL_DOOR
						);
						// Wall Base Top
						GridAddMesh(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									240.f)
							),
							STAGE_GRID_MESH_CORRIDOR,
							STAGE_GRID_CORRIDOR_WALL_BASE
						);
						//if (Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS) bCreatedDoorForBoosRoom = true;

						// ArchWall To Room
						GridAddMesh(
							FTransform(
								FRotator(0.f, -90.f + 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									300.f)
							),
							STAGE_GRID_MESH_CORRIDOR,
							STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
						);
					}
					// Ceiling * 4
					GridAddMesh(
						FTransform(
							FRotator(0.f, 90.f * d, 0.f),
							FVector(
								i * 100.f * Stage_Scale,
								j * 100.f * Stage_Scale,
								0.f)
						),
						STAGE_GRID_MESH_CORRIDOR,
						STAGE_GRID_CORRIDOR_CEILING1
					);
					d++;
				}

				// Philar * 2
				GridAddMesh(
					FTransform(
						FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
						FVector(
							i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
							j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
							0.f)
					),
					STAGE_GRID_MESH_CORRIDOR,
					STAGE_GRID_CORRIDOR_PHILAR
				);
				GridAddMesh(
					FTransform(
						FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
						FVector(
							i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
							j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
							0.f)
					),
					STAGE_GRID_MESH_CORRIDOR,
					STAGE_GRID_CORRIDOR_PHILAR
				);
			}	// Room Generate
			else if (Stage_Room_Coord[i][j].State == ROOM_EDGE)
			{
				IIStageGrid_Meshes* GR = Stage_Room_Coord[i][j].Generated_Room;
				if (GR != nullptr)
				{
					// Floor
					GR->AddGrid(
						FTransform(
							FVector(
								i * 100.f * Stage_Scale,
								j * 100.f * Stage_Scale,
								0.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					// Ceiling
					GR->AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 180.f),
							FVector(
								i * 100.f * Stage_Scale,
								j * 100.f * Stage_Scale,
								500.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					int d = 0;
					for (const FCoordinate& dir : Directions)
					{
						FCoordinate tempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, dir);
						FCoordinate tempCoord_90 = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 1) % 4]);
						FCoordinate tempCoord_270 = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 3) % 4]);

						bool bIsDirBlockCeling = (
							(
								!IsCoordinateInBound(tempCoord_90)) ||
							(
									Stage_Room_Coord[tempCoord_90.Get<0>()][tempCoord_90.Get<1>()].State == ROOM_CORRIDOR ||
									Stage_Room_Coord[tempCoord_90.Get<0>()][tempCoord_90.Get<1>()].State == ROOM_BLANK
								) ? true : false);
						bool bIsDirBlockCeling_R = (
							(
								!IsCoordinateInBound(tempCoord_270)) ||
							(
								Stage_Room_Coord[tempCoord_270.Get<0>()][tempCoord_270.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_270.Get<0>()][tempCoord_270.Get<1>()].State == ROOM_BLANK
								) ? true : false);


						// Spawn Wall
						if (!IsCoordinateInBound(tempCoord) || (
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BLANK ||
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_CORRIDOR
								) &&
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_BOSS &&
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_BOSS_DOOR)
							))
						{
							// Skip Entrance
							if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
							{ 
								//Wall Base Bottom
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											0.f)
									),
									STAGE_GRID_CORRIDOR_WALL_BASE
								);
								// Wall
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											0.f)
									),
									STAGE_GRID_CORRIDOR_WALL
								);
								// Wall Base Top
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											240.f)
									),
									STAGE_GRID_CORRIDOR_WALL_BASE
								);
								// Spawn Candlelabra
								if (!bIsDirBlockCeling && !bIsDirBlockCeling_R)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
												180.f)
										),
										STAGE_GRID_CORRIDOR_CANDLELABRA
									);
									Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 72.f,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 72.f,
										180.f + 66.f));
									Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 54.f,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 54.f,
										180.f + 127.f));
									Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
									Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
								}
							}
							// Philar Edge * 2
							if (d == 0)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING1
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
								if (!bIsDirBlockCeling)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING1
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -180.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
							}
							else if (d == 1)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
								}
								// Spawn Ceiling Corner
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									),
									STAGE_GRID_CORRIDOR_CEILING1
								);
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 180.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									),
									STAGE_GRID_CORRIDOR_CEILING1
								);
								if (!bIsDirBlockCeling && GR != nullptr)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
								if (!bIsDirBlockCeling_R && GR != nullptr)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 270.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
							}
							else if (d == 2)
							{
								if(!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING1
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
								if (!bIsDirBlockCeling)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING1
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -180.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
							}
							else if (d == 3)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
								}
								// Spawn Ceiling Corner
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									),
									STAGE_GRID_CORRIDOR_CEILING1
								);
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 180.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									),
									STAGE_GRID_CORRIDOR_CEILING1
								);
								if (!bIsDirBlockCeling)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
							}
							// Wall Arch * 4
							GR->AddGrid(
								FTransform(
									FRotator(0.f, -90.f + 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										300.f)
								),
								STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
							);
						}
						d++;
					}

					bool bIsSurroundedByAnyTiles = true;
					for (const FCoordinate& sub_dir : Directions)
					{
						FCoordinate subtempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, sub_dir);
						if (!IsCoordinateInBound(subtempCoord) ||
							Stage_Room_Coord[subtempCoord.Get<0>()][subtempCoord.Get<1>()].State != ROOM_EDGE)
						{
							bIsSurroundedByAnyTiles = false;
							break;
						}
					}
					if (bIsSurroundedByAnyTiles)
					{
						for(int subd = 0; subd < 4; subd++)
						{
							GR->AddGrid(
								FTransform(
									FRotator(0.f, 90.f * subd, 0.f),
									FVector(
										i * 100.f * Stage_Scale,
										j * 100.f * Stage_Scale,
										0.f)
								),
								STAGE_GRID_CORRIDOR_CEILING2
							);
						}
					}
				}
			}
			else if (Stage_Room_Coord[i][j].State == ROOM_BOSS || Stage_Room_Coord[i][j].State == ROOM_BOSS_DOOR)
			{
				IIStageGrid_Meshes* GR = Stage_Room_Coord[i][j].Generated_Room;
				if (GR != nullptr)
				{
					// Floor
					GR->AddGrid(
						FTransform(
							FVector(
								i * 100.f * Stage_Scale,
								j * 100.f * Stage_Scale,
								0.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					// Ceiling
					GR->AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 180.f),
							FVector(
								i * 100.f * Stage_Scale,
								j * 100.f * Stage_Scale,
								500.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					int d = 0;
					for (const FCoordinate& dir : Directions)
					{
						FCoordinate tempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, dir);
						FCoordinate tempCoord_90 = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 1) % 4]);
						FCoordinate tempCoord_270 = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 3) % 4]);
						FCoordinate tempCoord_90_2 = SumCoordinate(SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 1) % 4]), Directions[(d + 1) % 4]);
						FCoordinate tempCoord_270_2 = SumCoordinate(SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 3) % 4]), Directions[(d + 3) % 4]);

						bool bIsDirBlockCeling = (
							(
								!IsCoordinateInBound(tempCoord_90)) ||
							(
								Stage_Room_Coord[tempCoord_90.Get<0>()][tempCoord_90.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_90.Get<0>()][tempCoord_90.Get<1>()].State == ROOM_BLANK
								) ? true : false);
						bool bIsDirBlockCeling2 = (
							(
								!IsCoordinateInBound(tempCoord_90_2)) ||
							(
								Stage_Room_Coord[tempCoord_90_2.Get<0>()][tempCoord_90_2.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_90_2.Get<0>()][tempCoord_90_2.Get<1>()].State == ROOM_BLANK
								) ? true : false);
						bool bIsDirBlockCeling_R = (
							(
								!IsCoordinateInBound(tempCoord_270)) ||
							(
								Stage_Room_Coord[tempCoord_270.Get<0>()][tempCoord_270.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_270.Get<0>()][tempCoord_270.Get<1>()].State == ROOM_BLANK
								) ? true : false);

						bool bIsDirBlockCeling_R2 = (
							(
								!IsCoordinateInBound(tempCoord_270_2)) ||
							(
								Stage_Room_Coord[tempCoord_270_2.Get<0>()][tempCoord_270_2.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_270_2.Get<0>()][tempCoord_270_2.Get<1>()].State == ROOM_BLANK
								) ? true : false);
						// Spawn Wall
						if (!IsCoordinateInBound(tempCoord) || (
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BLANK ||
								(
									Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_CORRIDOR ||
									Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE
									)
								) &&
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_BOSS &&
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_BOSS_DOOR)
							))
						{
							// Spawn Candlelabra
							if (
								(Stage_Room_Coord[i][j].State != ROOM_BOSS_DOOR &&
									!(
										!bIsDirBlockCeling2 && !bIsDirBlockCeling_R2
									)) && (
								!bIsDirBlockCeling && !bIsDirBlockCeling_R)
								)
							{
								GR->AddGrid(
									FTransform(
										FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											180.f)
									),
									STAGE_GRID_CORRIDOR_CANDLELABRA
								);
								Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 72.f,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 72.f,
									180.f + 66.f));
								Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 54.f,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 54.f,
									180.f + 127.f));
								Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
								Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
							}

							// Skip Entrance
							if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
							{
								//Wall Base Bottom
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											0.f)
									),
									STAGE_GRID_CORRIDOR_WALL_BASE
								);
								// Wall
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											0.f)
									),
									STAGE_GRID_CORRIDOR_WALL
								);
								// Wall Base Top
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											240.f)
									),
									STAGE_GRID_CORRIDOR_WALL_BASE
								);
							}
							// Philar Edge * 2
							if (d == 0)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING1
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
								if (!bIsDirBlockCeling)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING1
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -180.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
							}
							else if (d == 1)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
								}
								// Spawn Ceiling Corner
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									),
									STAGE_GRID_CORRIDOR_CEILING1
								);
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 180.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									),
									STAGE_GRID_CORRIDOR_CEILING1
								);
								if (!bIsDirBlockCeling)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 270.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
							}
							else if (d == 2)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING1
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
								if (!bIsDirBlockCeling)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING1
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -180.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
							}
							else if (d == 3)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
									GR->AddGrid(
										FTransform(
											FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
											FVector(
												i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
												0.f)
										),
										STAGE_GRID_CORRIDOR_PHILAR
									);
								}
								// Spawn Ceiling Corner
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 90.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									),
									STAGE_GRID_CORRIDOR_CEILING1
								);
								GR->AddGrid(
									FTransform(
										FRotator(0.f, 180.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									),
									STAGE_GRID_CORRIDOR_CEILING1
								);
								if (!bIsDirBlockCeling)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									GR->AddGrid(
										FTransform(
											FRotator(0.f, -90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										),
										STAGE_GRID_CORRIDOR_CEILING2
									);
								}
							}
							// Wall Arch * 4
							GR->AddGrid(
								FTransform(
									FRotator(0.f, -90.f + 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										300.f)
								),
								STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
							);
						}
						d++;
					}
					bool bIsSurroundedByAnyTiles = true;
					for (const FCoordinate& sub_dir : Directions)
					{
						FCoordinate subtempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, sub_dir);
						if (!IsCoordinateInBound(subtempCoord) ||
							(
								Stage_Room_Coord[subtempCoord.Get<0>()][subtempCoord.Get<1>()].State != ROOM_BOSS &&
								Stage_Room_Coord[subtempCoord.Get<0>()][subtempCoord.Get<1>()].State != ROOM_BOSS_DOOR
								))
						{
							bIsSurroundedByAnyTiles = false;
							break;
						}
					}
					if (bIsSurroundedByAnyTiles)
					{
						for (int subd = 0; subd < 4; subd++)
						{
							GR->AddGrid(
								FTransform(
									FRotator(0.f, 90.f * subd, 0.f),
									FVector(
										i * 100.f * Stage_Scale,
										j * 100.f * Stage_Scale,
										0.f)
								),
								STAGE_GRID_CORRIDOR_CEILING2
							);
						}
					}
				}
			}

			//
			// DEBUG
			//
			SpawnWall(
				FVector(
					-100 * Stage_Scale * Coord_Height + i * 100.f * Stage_Scale,
					-100 * Stage_Scale * Coord_Width + j * 100.f * Stage_Scale,
					100.f * Stage_Scale * Stage_Room_Coord[i][j].Floor
				), Stage_Room_Coord[i][j].State,
				Stage_Room_Coord[i][j].DistFromEntrance
			);
		}
	}*/
}

void ACGeneratedStage::ClearStage()
{
	Stage_Room_Coord.Empty();
	Stage_Rooms.Empty();

	for (int32 i = 0; i < STAGE_GRID_MESH_TYPES; i++)
	{
		IIStageGrid_Meshes* ISGM = Cast<IIStageGrid_Meshes>(Stage_Grid_Meshes[i]);
		if (ISGM == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("ACGeneratedStage : ClearStage : Fail To Clear Stage : Grid Mes Type : %d"), i);
			continue;
		}
		ISGM->ClearGrid();
	}
}

bool ACGeneratedStage::Stage_CalculateClosestCorridor(TArray<FCoordinate>& Trail)
{
	// BFS
	FCoordinate Start = Trail.Last();
	FCoordinate Destination = Start;
	int32 Start_Root = Room_Find(Stage_Room_Coord[Start.Get<0>()][Start.Get<1>()].Root);
	Trail.Pop();

	TQueue<FCoordinate> Q;
	TSet<FCoordinate> Visited;
	TMap<FCoordinate, FCoordinate> From;

	Q.Enqueue(Start);
	Visited.Add(Start);

	bool Flag = false;
	while (!Q.IsEmpty())
	{
		FCoordinate Curr;
		Q.Dequeue(Curr);

		for (const FCoordinate& d : Directions)
		{
			FCoordinate Node = FCoordinate{
				Curr.Get<0>() + d.Get<0>(),
				Curr.Get<1>() + d.Get<1>(),
				Curr.Get<2>() + d.Get<2>(),
			};
			int32 x = Node.Get<0>();
			int32 y = Node.Get<1>();

			if ((x < 0 || y < 0) || (x >= Coord_Height || y >= Coord_Width)) continue;
			if (Visited.Contains(Node)) continue;
			if (Stage_Room_Coord[x][y].State == ROOM_BOSS) continue;
			if (Stage_Room_Coord[x][y].State == ROOM_STAIR) continue;
			if (Stage_Room_Coord[x][y].State == ROOM_DEBUG_BLOCK) continue;

			Visited.Add(Node);
			From.Add(Node, Curr);

			bool bIsSelf;
			if (Stage_Room_Coord[x][y].Root >= 0) bIsSelf = Start_Root == Room_Find(Stage_Room_Coord[x][y].Root) ? true : false;
			else bIsSelf = false;
			if (Stage_Room_Coord[x][y].State == ROOM_BLANK || bIsSelf) Q.Enqueue(Node);

			bool bIsSomething = (
				(Stage_Room_Coord[x][y].State == ROOM_CORRIDOR || Stage_Room_Coord[x][y].State == ROOM_OCCUPIED) || Stage_Room_Coord[x][y].State == ROOM_BOSS_DOOR
				) ? true : false;
			if (!bIsSelf && bIsSomething)
			{
				Destination = Node;
				Flag = true;
				break;
			}
		}
		if (Flag) break;
	}

	if (Flag)
	{
		while (true)
		{
			Trail.Add(Destination);
			if (Destination == Start) break;
			Destination = From[Destination];
		}
	}
	return Flag;
}

bool ACGeneratedStage::Stage_FindTwoClosestPointsBetweenTwoNodes(int32 RootA, int32 RootB, FCoordinate& outA, FCoordinate& outB)
{
	FCoordinate CoordA = Stage_Rooms[RootA]->Coordinate;
	FCoordinate CoordB = Stage_Rooms[RootB]->Coordinate;
	outA = CoordA;
	outB = CoordB;
	int32 Dist = Room_GetManhattan(CoordA, CoordB);

	while (true)
	{
		for (const FCoordinate& dirA : Directions)
		{
			FCoordinate tempCoordA = SumCoordinate(CoordA, dirA);
			if (!IsCoordinateInBound(tempCoordA)) continue;
			for (const FCoordinate& dirB : Directions)
			{
				FCoordinate tempCoordB = SumCoordinate(CoordB, dirB);
				if (!IsCoordinateInBound(tempCoordB)) continue;

				int32 tempDist = Room_GetManhattan(tempCoordA, tempCoordB);
				if (tempDist < Dist)
				{
					Dist = tempDist;
					outA = tempCoordA;
					outB = tempCoordB;
				}
			}
		}
	}
	return false;
}

bool ACGeneratedStage::Stage_CalculateCorridorBetweenTwoNodes(TArray<FCoordinate>& Trail, FCoordinate Start, FCoordinate Destination)
{
	using FNodeInfo = TPair<FCoordinate, int32>;
	// A*
	auto DistPred = [](const TPair<FCoordinate, int32>& A, const TPair<FCoordinate, int32>& B) {
		return A.Value < B.Value;
		};

	/*Coord, Dist*/
	TArray<FNodeInfo> UnVisited;
	TMap<FCoordinate, FCoordinate> From;
	TMap<FCoordinate, int32> gScore;
	//TMap<FCoordinate, int32> fScore;

	UnVisited.HeapPush(FNodeInfo(Start, Room_GetManhattan(Start, Destination)), DistPred);
	gScore.Add(Start, 0);
	//fScore.Add(Start, Room_GetManhattan(Start, Destination));

	while (!UnVisited.IsEmpty())
	{
		FNodeInfo Curr_Node{};
		UnVisited.HeapPop(Curr_Node, DistPred);
		FCoordinate Curr = Curr_Node.Key;
		if (Curr == Destination) break;

		for (const FCoordinate& dir : Directions)
		{
			FCoordinate Node{
				Curr.Get<0>() + dir.Get<0>(),
				Curr.Get<1>() + dir.Get<1>(),
				Curr.Get<2>() + dir.Get<2>()
			};
			int32 x = Node.Get<0>();
			int32 y = Node.Get<1>();
			if ((x < 0 || y < 0) || (x >= Coord_Height || y >= Coord_Width)) continue;

			int32 temp_gScore = gScore[Curr] + (
				Stage_Room_Coord[Node.Get<0>()][Node.Get<1>()].State == ROOM_BLANK ? 1 : 0
				);

			if (!gScore.Contains(Node) || temp_gScore < gScore[Node])
			{
				gScore.Add(Node, temp_gScore);
				UnVisited.HeapPush(
					FNodeInfo(
						Node,
						gScore[Node] + Room_GetManhattan(Node, Destination)
					)
				);
				From.Add(Node, Curr);
			}
		}
	}

	FCoordinate CurrentNode = Destination;
	int MaxAttempt = Coord_Width * Coord_Height;
	int Trial = 0;
	while (From.Contains(CurrentNode) && CurrentNode != Start)
	{
		Trail.Add(CurrentNode);
		CurrentNode = From[CurrentNode];
		Trial++;
		if (Trial > MaxAttempt)
		{
			UE_LOG(LogTemp, Error, TEXT("ACGeneratedStage : Stage_CalculateCorridorBetweenTwoNodes : Could Not Create Corridor"));
			return false;
		}
	}
	return true;
}

FCoordinate ACGeneratedStage::SumCoordinate(FCoordinate A, FCoordinate B)
{
	return FCoordinate{
		A.Get<0>() + B.Get<0>(),
		A.Get<1>() + B.Get<1>(),
		A.Get<2>() + B.Get<2>()
	};
}

bool ACGeneratedStage::IsCoordinateInBound(FCoordinate A)
{
	return (A.Get<0>() < 0 || A.Get<1>() < 0) || (A.Get<0>() >= Coord_Height || A.Get<1>() >= Coord_Width) ? false : true;
}

void ACGeneratedStage::Stage_GridGenerate()
{
	bCreatedStageCoordinates = true;
	/*
		Spawn Rooms
	*/
	for (int i = 0; i < Coord_Height; i++)
	{
		for (int j = 0; j < Coord_Width; j++)
		{
			if (Stage_Room_Coord[i][j].State == ROOM_BLANK) continue;
			else if (Stage_Room_Coord[i][j].State == ROOM_CORRIDOR || Stage_Room_Coord[i][j].State == ROOM_OCCUPIED)
			{
				// Debug

				UTextRenderComponent* TextRender = NewObject<UTextRenderComponent>(this);
				TextRender->RegisterComponent();
				//TextRender->AttachToComponent(SMC, FAttachmentTransformRules::SnapToTargetIncludingScale);
				TextRender->SetRelativeScale3D(FVector(4.f, 4.f, 4.f));
				TextRender->SetRelativeRotation(FRotator(90.f, 0.f, 180.f));
				TextRender->SetRelativeLocation(GetActorLocation() + (FVector(i * 100.f * Stage_Scale, j * 100.f * Stage_Scale, 0.f) + FVector(50.f, 50.f, 110.f)));
				TextRender->SetText(FText::FromString(FString::FromInt(Stage_Room_Coord[i][j].DistFromEntrance)));
				Stage_Room_Coord[i][j].DebugTextComponent = TextRender;

				// Place Chest
				if (Stage_Room_Coord[i][j].State == ROOM_OCCUPIED && ObjectPoolManager != nullptr)
				{
					int32 PlaceDirection = 0;
					int32 ChestDist = Stage_Room_Coord[i][j].DistFromEntrance;
					for (int32 d = 0; d < 4; d++)
					{
						FCoordinate dir = Directions[d];
						FCoordinate tempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, dir);
						if (!IsCoordinateInBound(tempCoord)) continue;
						if (Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_CORRIDOR && 
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].DistFromEntrance < ChestDist)
						{
							ChestDist = Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].DistFromEntrance;
							PlaceDirection = d;
						}
					}
					//IIInteractableItem* tempChest = ObjectPoolManager->GetChest(this, FTransform(
					//	FRotator(0.f, -90.f * PlaceDirection, 0.f),
					//	FVector(i * 100.f * Stage_Scale, j * 100.f * Stage_Scale, 0.f)
					//));
					int32 item_gen_num_max = FMath::FloorToInt32(FMath::FRandRange(1.8f, 9.f));
					for (int32 item_gen_num = 0; item_gen_num < item_gen_num_max; item_gen_num++)
					{
						Stage_Room_Coord[i][j].ChestSpawnItems.Add(FMath::FloorToInt32(FMath::FRandRange(0.f, INTERACTABLE_ITEM_NUM)));
					}
					Stage_Room_Coord[i][j].bChestOpened = false;
				}
				// Floor
				Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
					ObjectPoolManager->GetStaticMeshComponent(
						this, STAGE_GRID_CORRIDOR_FLOOR, FTransform(
							FVector(i * 100.f * Stage_Scale, j * 100.f * Stage_Scale, 0.f)
						)
					), STAGE_GRID_CORRIDOR_FLOOR)
				);
				// Ceiling
				Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
					ObjectPoolManager->GetStaticMeshComponent(
						this, STAGE_GRID_CORRIDOR_FLOOR, FTransform(
							FRotator(0.f, 0.f, 180.f),
							FVector(i * 100.f * Stage_Scale, j * 100.f * Stage_Scale, 500.f)
						)
					), STAGE_GRID_CORRIDOR_FLOOR)
				);

				int d = 0;
				for (const FCoordinate& dir : Directions)
				{
					FCoordinate tempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, dir);

					// Candle
					// Create Candle Every 4th Grid

					// Can Not Be Placed Because There's No Wall
					bool bCreateCandleReverse = false;
					if (Stage_Room_Coord[i][j].DistFromEntrance > 0 &&
						(Stage_Room_Coord[i][j].DistFromEntrance - 1) % 2 == 0)
					{
						FCoordinate way0coord = SumCoordinate(
							Stage_Room_Coord[i][j].Coordinate, Directions[((Stage_Room_Coord[i][j].DistFromEntrance - 1) % 4 == 0) ? 0 : 2]
							);
						FCoordinate way1coord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[((Stage_Room_Coord[i][j].DistFromEntrance - 1) % 4 == 0) ? 1 : 3]);
						bool CreateAt0 = false;
						bool CreateAt1 = false;
						if ((!IsCoordinateInBound(way0coord) || (
							(
								(
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].State == ROOM_BLANK ||
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].State == ROOM_BOSS
									) ||
								(
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].State == ROOM_STAIR
									)
								) ||
							(
								Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].State == ROOM_EDGE && (
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].Generated_Room != nullptr &&
									Stage_Room_Coord[way0coord.Get<0>()][way0coord.Get<1>()].Generated_Room->GetHasDoor()
									)
								)
							)))
						{
							CreateAt0 = true;
						}
						if ((!IsCoordinateInBound(way1coord) || (
							(
								(
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].State == ROOM_BLANK ||
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].State == ROOM_BOSS
									) ||
								(
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].State == ROOM_STAIR
									)
								) ||
							(
								Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].State == ROOM_EDGE && (
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].Generated_Room != nullptr &&
									Stage_Room_Coord[way1coord.Get<0>()][way1coord.Get<1>()].Generated_Room->GetHasDoor()
									)
								)
							)))
						{
							CreateAt1 = true;
						}
						if (!CreateAt0 && !CreateAt1)
						{
							bCreateCandleReverse = true;
						}

						if (((Stage_Room_Coord[i][j].DistFromEntrance - 1) % 4 == 0 && ((!bCreateCandleReverse && (d == 0 || d == 1)) || (bCreateCandleReverse && (d == 3 || d == 2)))) ||
							(Stage_Room_Coord[i][j].DistFromEntrance - 1) % 4 != 0 && ((!bCreateCandleReverse && (d == 3 || d == 2)) || (bCreateCandleReverse && (d == 0 || d == 1))))
						{
							if (!IsCoordinateInBound(tempCoord) || (
								(
									(
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BLANK ||
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS
										) ||
									(
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_STAIR
										)
									) ||
								(
									Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE && (
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room != nullptr &&
										Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room->GetHasDoor()
										)
									)
								))
							{
								// Spawn Candlelabra
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CANDLELABRA, FTransform(
												FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
													180.f)
										)
									), STAGE_GRID_CORRIDOR_CANDLELABRA)
								);
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CANDLE, FTransform(
											FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 72.f,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 72.f,
												180.f + 24.f
											)
										)
									), STAGE_GRID_CORRIDOR_CANDLE)
								);
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CANDLEFLAME, FTransform(
											FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 72.f,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 72.f,
												180.f + 24.f + 50.f
											)
										)
									), STAGE_GRID_CORRIDOR_CANDLEFLAME)
								);
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CANDLE, FTransform(
											FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 54.f,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 54.f,
												180.f + 85.f
											)
										)
									), STAGE_GRID_CORRIDOR_CANDLE)
								);
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CANDLEFLAME, FTransform(
											FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 54.f,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 54.f,
												180.f + 85.f + 50.f
											)
										)
									), STAGE_GRID_CORRIDOR_CANDLEFLAME)
								);
								Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 72.f,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 72.f,
									180.f + 66.f + 10.f));
								Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 54.f,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 54.f,
									180.f + 127.f + 10.f));
								Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
								Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
							}
						}
					}

					// Spawn Wall
					if (!IsCoordinateInBound(tempCoord) || (
						(
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BLANK ||
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS
								) ||
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_STAIR
								)
							) ||
						(
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE && (
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room != nullptr &&
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room->GetHasDoor()
								)
							)
						))
					{
						//Wall Base Bottom
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_WALL_BASE, FTransform(
									FRotator(0.f, 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										0.f)
								)
							), STAGE_GRID_CORRIDOR_WALL_BASE)
						);
						// Wall
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_WALL, FTransform(
									FRotator(0.f, 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										0.f)
								)
							), STAGE_GRID_CORRIDOR_WALL)
						);
						// Wall Base Top
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_WALL_BASE, FTransform(
									FRotator(0.f, 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										240.f)
								)
							), STAGE_GRID_CORRIDOR_WALL_BASE)
						);

						// Philar Edge * 2
						if (d == 3)
						{
							Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
								ObjectPoolManager->GetStaticMeshComponent(
									this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
										FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
										FVector(
											i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
											0.f)
									)
								), STAGE_GRID_CORRIDOR_PHILAR)
							);
							Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
								ObjectPoolManager->GetStaticMeshComponent(
									this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
										FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
										FVector(
											i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
											0.f)
									)
								), STAGE_GRID_CORRIDOR_PHILAR)
							);
						}
						// Wall Arch * 4
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL, FTransform(
									FRotator(0.f, -90.f + 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										300.f)
								)
							), STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL)
						);
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_CEILING_ARCH, FTransform(
									FRotator(0.f, -90.f + 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										300.f)
								)
							), STAGE_GRID_CORRIDOR_CEILING_ARCH)
						);
					} // Spawn Door Not Wall
					else if (IsCoordinateInBound(tempCoord) && (
						(
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_OCCUPIED ||
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_STAIR_DOOR
							) || (
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE ||
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS_DOOR
							)
						)
						)
					{
						bool bIsBossRoom = Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS ? true : false;
						if ((Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE ||
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BOSS_DOOR) &&
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room != nullptr)
						{
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room->SetHasDoor(true);
							Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].Generated_Room->SetDoorLocation(
								FVector(
									tempCoord.Get<0>() * 100.f * Stage_Scale,
									tempCoord.Get<1>() * 100.f * Stage_Scale,
									GetActorLocation().Z)
							);
						}
						//Wall Door Base Bottom
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_WALL_DOOR_BASE, FTransform(
									FRotator(0.f, 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										0.f)
								)
							), STAGE_GRID_CORRIDOR_WALL_DOOR_BASE)
						);
						// Actor Door
						Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].DoorTransformArr.Add(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
									0.f)
							)
						);
						Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].DoorArr.Add(nullptr);
						Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].DoorOpenStateArr.Add(INTERACTABLE_ITEM_STATE_CLOSED);
						// Wall Door
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_WALL_DOOR, FTransform(
									FRotator(0.f, 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										0.f)
								)
							), STAGE_GRID_CORRIDOR_WALL_DOOR)
						);
						// Wall Base Top
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_WALL_BASE, FTransform(
									FRotator(0.f, 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										240.f)
								)
							), STAGE_GRID_CORRIDOR_WALL_BASE)
						);
						// ArchWall To Room
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL, FTransform(
									FRotator(0.f, -90.f + 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										300.f)
								)
							), STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL)
						);
						Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
							ObjectPoolManager->GetStaticMeshComponent(
								this, STAGE_GRID_CORRIDOR_CEILING_ARCH, FTransform(
									FRotator(0.f, -90.f + 90.f * d, 0.f),
									FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
										300.f)
								)
							), STAGE_GRID_CORRIDOR_CEILING_ARCH)
						);
					}
					// Ceiling * 4
					Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
						ObjectPoolManager->GetStaticMeshComponent(
							this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(
									i * 100.f * Stage_Scale,
									j * 100.f * Stage_Scale,
									0.f)
							)
						), STAGE_GRID_CORRIDOR_CEILING1)
					);
					d++;
				}

				// Philar * 2
				Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
					ObjectPoolManager->GetStaticMeshComponent(
						this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
								j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
								0.f)
						)
					), STAGE_GRID_CORRIDOR_PHILAR)
				);
				Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
					ObjectPoolManager->GetStaticMeshComponent(
						this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
								j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
								0.f)
						)
					), STAGE_GRID_CORRIDOR_PHILAR)
				);
			}	// Room Generate
			else if (Stage_Room_Coord[i][j].State == ROOM_EDGE)
			{
				IIStageGrid_Room* GR = Stage_Room_Coord[i][j].Generated_Room;
				GetWorld()->SpawnActor<ACMinion>(ACMinion::StaticClass(), FTransform(
					FVector(
						i * 100.f * Stage_Scale,
						j * 100.f * Stage_Scale,
						GetActorLocation().Z + 88.f)
				)
				);
				if (GR != nullptr)
				{
					// Floor
					Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
						ObjectPoolManager->GetStaticMeshComponent(
							this, STAGE_GRID_CORRIDOR_FLOOR, FTransform(
								FVector(
									i * 100.f * Stage_Scale,
									j * 100.f * Stage_Scale,
									0.f)
							)
						), STAGE_GRID_CORRIDOR_FLOOR)
					);
					// Ceiling
					Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
						ObjectPoolManager->GetStaticMeshComponent(
							this, STAGE_GRID_CORRIDOR_FLOOR, FTransform(
								FRotator(0.f, 0.f, 180.f),
								FVector(
									i * 100.f * Stage_Scale,
									j * 100.f * Stage_Scale,
									500.f)
							)
						), STAGE_GRID_CORRIDOR_FLOOR)
					);
					int d = 0;
					for (const FCoordinate& dir : Directions)
					{
						FCoordinate tempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, dir);
						FCoordinate tempCoord_90 = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 1) % 4]);
						FCoordinate tempCoord_270 = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 3) % 4]);

						bool bIsDirBlockCeling = (
							(
								!IsCoordinateInBound(tempCoord_90)) ||
							(
								Stage_Room_Coord[tempCoord_90.Get<0>()][tempCoord_90.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_90.Get<0>()][tempCoord_90.Get<1>()].State == ROOM_BLANK
								) ? true : false);
						bool bIsDirBlockCeling_R = (
							(
								!IsCoordinateInBound(tempCoord_270)) ||
							(
								Stage_Room_Coord[tempCoord_270.Get<0>()][tempCoord_270.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_270.Get<0>()][tempCoord_270.Get<1>()].State == ROOM_BLANK
								) ? true : false);


						// Spawn Wall
						if (!IsCoordinateInBound(tempCoord) || (
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BLANK ||
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_CORRIDOR
								) &&
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_BOSS &&
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_BOSS_DOOR)
							))
						{
							// Skip Entrance
							if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
							{
								//Wall Base Bottom
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_WALL_BASE, FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_WALL_BASE)
								);
								// Wall
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_WALL, FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_WALL)
								);
								// Wall Base Top
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_WALL_BASE, FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
												240.f)
										)
									), STAGE_GRID_CORRIDOR_WALL_BASE)
								);
								// Spawn Candlelabra
								if (!bIsDirBlockCeling && !bIsDirBlockCeling_R)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CANDLELABRA, FTransform(
												FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
													180.f)
											)
										), STAGE_GRID_CORRIDOR_CANDLELABRA)
									);
									Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 72.f,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 72.f,
										180.f + 66.f));
									Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
										i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 54.f,
										j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 54.f,
										180.f + 127.f));
									Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
									Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
								}
							}
							// Philar Edge * 2
							if (d == 0)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
												FRotator(0.f, 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING1)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
								if (!bIsDirBlockCeling)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
												FRotator(0.f, -90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING1)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, -180.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
							}
							else if (d == 1)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
								}
								// Spawn Ceiling Corner
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
											FRotator(0.f, 90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_CEILING1)
								);
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
											FRotator(0.f, 180.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_CEILING1)
								);
								if (!bIsDirBlockCeling && GR != nullptr)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
								if (!bIsDirBlockCeling_R && GR != nullptr)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 270.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
							}
							else if (d == 2)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
												FRotator(0.f, 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING1)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
								if (!bIsDirBlockCeling)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
												FRotator(0.f, -90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING1)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, -180.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
							}
							else if (d == 3)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
								}
								// Spawn Ceiling Corner
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
											FRotator(0.f, 90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_CEILING1)
								);
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
											FRotator(0.f, 180.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_CEILING1)
								);
								if (!bIsDirBlockCeling)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, -90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
							}
							// Wall Arch * 4
							Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
								ObjectPoolManager->GetStaticMeshComponent(
									this, STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL, FTransform(
										FRotator(0.f, -90.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											300.f)
									)
								), STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL)
							);
							Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
								ObjectPoolManager->GetStaticMeshComponent(
									this, STAGE_GRID_CORRIDOR_CEILING_ARCH, FTransform(
										FRotator(0.f, -90.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											300.f)
									)
								), STAGE_GRID_CORRIDOR_CEILING_ARCH)
							);
						}
						d++;
					}

					bool bIsSurroundedByAnyTiles = true;
					for (const FCoordinate& sub_dir : Directions)
					{
						FCoordinate subtempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, sub_dir);
						if (!IsCoordinateInBound(subtempCoord) ||
							Stage_Room_Coord[subtempCoord.Get<0>()][subtempCoord.Get<1>()].State != ROOM_EDGE)
						{
							bIsSurroundedByAnyTiles = false;
							break;
						}
					}
					if (bIsSurroundedByAnyTiles)
					{
						for (int subd = 0; subd < 4; subd++)
						{
							Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
								ObjectPoolManager->GetStaticMeshComponent(
									this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
										FRotator(0.f, 90.f * subd, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									)
								), STAGE_GRID_CORRIDOR_CEILING2)
							);
						}
					}
				}
			}
			else if (Stage_Room_Coord[i][j].State == ROOM_BOSS || Stage_Room_Coord[i][j].State == ROOM_BOSS_DOOR)
			{
				IIStageGrid_Room* GR = Stage_Room_Coord[i][j].Generated_Room;
				if (GR != nullptr)
				{
					// Floor
					Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
						ObjectPoolManager->GetStaticMeshComponent(
							this, STAGE_GRID_CORRIDOR_FLOOR, FTransform(
								FVector(
									i * 100.f * Stage_Scale,
									j * 100.f * Stage_Scale,
									0.f)
							)
						), STAGE_GRID_CORRIDOR_FLOOR)
					);
					// Ceiling
					Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
						ObjectPoolManager->GetStaticMeshComponent(
							this, STAGE_GRID_CORRIDOR_FLOOR, FTransform(
								FRotator(0.f, 0.f, 180.f),
								FVector(
									i * 100.f * Stage_Scale,
									j * 100.f * Stage_Scale,
									500.f)
							)
						), STAGE_GRID_CORRIDOR_FLOOR)
					);
					int d = 0;
					for (const FCoordinate& dir : Directions)
					{
						FCoordinate tempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, dir);
						FCoordinate tempCoord_90 = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 1) % 4]);
						FCoordinate tempCoord_270 = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 3) % 4]);
						FCoordinate tempCoord_90_2 = SumCoordinate(SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 1) % 4]), Directions[(d + 1) % 4]);
						FCoordinate tempCoord_270_2 = SumCoordinate(SumCoordinate(Stage_Room_Coord[i][j].Coordinate, Directions[(d + 3) % 4]), Directions[(d + 3) % 4]);

						bool bIsDirBlockCeling = (
							(
								!IsCoordinateInBound(tempCoord_90)) ||
							(
								Stage_Room_Coord[tempCoord_90.Get<0>()][tempCoord_90.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_90.Get<0>()][tempCoord_90.Get<1>()].State == ROOM_BLANK
								) ? true : false);
						bool bIsDirBlockCeling2 = (
							(
								!IsCoordinateInBound(tempCoord_90_2)) ||
							(
								Stage_Room_Coord[tempCoord_90_2.Get<0>()][tempCoord_90_2.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_90_2.Get<0>()][tempCoord_90_2.Get<1>()].State == ROOM_BLANK
								) ? true : false);
						bool bIsDirBlockCeling_R = (
							(
								!IsCoordinateInBound(tempCoord_270)) ||
							(
								Stage_Room_Coord[tempCoord_270.Get<0>()][tempCoord_270.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_270.Get<0>()][tempCoord_270.Get<1>()].State == ROOM_BLANK
								) ? true : false);

						bool bIsDirBlockCeling_R2 = (
							(
								!IsCoordinateInBound(tempCoord_270_2)) ||
							(
								Stage_Room_Coord[tempCoord_270_2.Get<0>()][tempCoord_270_2.Get<1>()].State == ROOM_CORRIDOR ||
								Stage_Room_Coord[tempCoord_270_2.Get<0>()][tempCoord_270_2.Get<1>()].State == ROOM_BLANK
								) ? true : false);
						// Spawn Wall
						if (!IsCoordinateInBound(tempCoord) || (
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_BLANK ||
								(
									Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_CORRIDOR ||
									Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_EDGE
									)
								) &&
							(
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_BOSS &&
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_BOSS_DOOR)
							))
						{
							// Spawn Candlelabra
							if (
								(Stage_Room_Coord[i][j].State != ROOM_BOSS_DOOR &&
									!(
										!bIsDirBlockCeling2 && !bIsDirBlockCeling_R2
										)) && (
											!bIsDirBlockCeling && !bIsDirBlockCeling_R)
								)
							{
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CANDLELABRA, FTransform(
											FRotator(0.f, (d % 2 < 1 ? 180.f : 0.f) + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
												180.f)
										)
									), STAGE_GRID_CORRIDOR_CANDLELABRA)
								);
								Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 72.f,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 72.f,
									180.f + 66.f));
								Stage_Room_Coord[i][j].CandlePosArr.Add(FVector(
									i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2 - dir.Get<0>() * 54.f,
									j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2 - dir.Get<1>() * 54.f,
									180.f + 127.f));
								Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
								Stage_Room_Coord[i][j].CandleLightArr.Add(nullptr);
							}

							// Skip Entrance
							if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
							{
								//Wall Base Bottom
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_WALL_BASE, FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_WALL_BASE)
								);
								// Wall
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_WALL, FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_WALL)
								);
								// Wall Base Top
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_WALL_BASE, FTransform(
											FRotator(0.f, 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
												j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
												240.f)
										)
									), STAGE_GRID_CORRIDOR_WALL_BASE)
								);
							}
							// Philar Edge * 2
							if (d == 0)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
												FRotator(0.f, 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING1)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
								if (!bIsDirBlockCeling)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
												FRotator(0.f, -90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING1)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, -180.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
							}
							else if (d == 1)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
								}
								// Spawn Ceiling Corner
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
											FRotator(0.f, 90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_CEILING1)
								);
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
											FRotator(0.f, 180.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_CEILING1)
								);
								if (!bIsDirBlockCeling)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 270.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
							}
							else if (d == 2)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
												FRotator(0.f, 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING1)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
								if (!bIsDirBlockCeling)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
												FRotator(0.f, -90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING1)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, -180.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
							}
							else if (d == 3)
							{
								if (!IsCoordinateInBound(tempCoord) || Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State != ROOM_CORRIDOR)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_PHILAR, FTransform(
												FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
												FVector(
													i * 100.f * Stage_Scale - 100.f * Stage_Scale / 2,
													j * 100.f * Stage_Scale + 100.f * Stage_Scale / 2,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_PHILAR)
									);
								}
								// Spawn Ceiling Corner
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
											FRotator(0.f, 90.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_CEILING1)
								);
								Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
									ObjectPoolManager->GetStaticMeshComponent(
										this, STAGE_GRID_CORRIDOR_CEILING1, FTransform(
											FRotator(0.f, 180.f + 90.f * d, 0.f),
											FVector(
												i * 100.f * Stage_Scale,
												j * 100.f * Stage_Scale,
												0.f)
										)
									), STAGE_GRID_CORRIDOR_CEILING1)
								);
								if (!bIsDirBlockCeling)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
								if (!bIsDirBlockCeling_R)
								{
									Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
										ObjectPoolManager->GetStaticMeshComponent(
											this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
												FRotator(0.f, -90.f + 90.f * d, 0.f),
												FVector(
													i * 100.f * Stage_Scale,
													j * 100.f * Stage_Scale,
													0.f)
											)
										), STAGE_GRID_CORRIDOR_CEILING2)
									);
								}
							}
							// Wall Arch * 4
							Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
								ObjectPoolManager->GetStaticMeshComponent(
									this, STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL, FTransform(
										FRotator(0.f, -90.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											300.f)
									)
								), STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL)
							);
							Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
								ObjectPoolManager->GetStaticMeshComponent(
									this, STAGE_GRID_CORRIDOR_CEILING_ARCH, FTransform(
										FRotator(0.f, -90.f + 90.f * d, 0.f),
										FVector(
											i * 100.f * Stage_Scale + dir.Get<0>() * 100.f * Stage_Scale / 2,
											j * 100.f * Stage_Scale + dir.Get<1>() * 100.f * Stage_Scale / 2,
											300.f)
									)
								), STAGE_GRID_CORRIDOR_CEILING_ARCH)
							);
						}
						d++;
					}
					bool bIsSurroundedByAnyTiles = true;
					for (const FCoordinate& sub_dir : Directions)
					{
						FCoordinate subtempCoord = SumCoordinate(Stage_Room_Coord[i][j].Coordinate, sub_dir);
						if (!IsCoordinateInBound(subtempCoord) ||
							(
								Stage_Room_Coord[subtempCoord.Get<0>()][subtempCoord.Get<1>()].State != ROOM_BOSS &&
								Stage_Room_Coord[subtempCoord.Get<0>()][subtempCoord.Get<1>()].State != ROOM_BOSS_DOOR
								))
						{
							bIsSurroundedByAnyTiles = false;
							break;
						}
					}
					if (bIsSurroundedByAnyTiles)
					{
						for (int subd = 0; subd < 4; subd++)
						{
							Stage_Room_Coord[i][j].SpawnedComponents.Add(FSpawnedStaticMeshComponent(
								ObjectPoolManager->GetStaticMeshComponent(
									this, STAGE_GRID_CORRIDOR_CEILING2, FTransform(
										FRotator(0.f, 90.f * subd, 0.f),
										FVector(
											i * 100.f * Stage_Scale,
											j * 100.f * Stage_Scale,
											0.f)
									)
								), STAGE_GRID_CORRIDOR_CEILING2)
							);
						}
					}
				}
			}

			//
			// DEBUG
			//
			//SpawnWall(
			//	FVector(
			//		-100 * Stage_Scale * Coord_Height + i * 100.f * Stage_Scale,
			//		-100 * Stage_Scale * Coord_Width + j * 100.f * Stage_Scale,
			//		100.f * Stage_Scale * Stage_Room_Coord[i][j].Floor
			//	), Stage_Room_Coord[i][j].State,
			//	Stage_Room_Coord[i][j].DistFromEntrance
			//);
		}
	}

	if (ObjectPoolManager != nullptr)
	{
		FTransform tempTransform{ FRotator::ZeroRotator, GetActorLocation() + FVector(200.f * Coord_Height, 200.f * Coord_Width, 0.f), FVector(Coord_Height + 2.f, Coord_Width + 2.f, 2.f) };
		ObjectPoolManager->SetNavMeshLocation(tempTransform);
		ObjectPoolManager->RebuildNavMesh();
	}
}

void ACGeneratedStage::Stage_GridReturn()
{
	if (ObjectPoolManager != nullptr)
	{
		for (int i = 0; i < Coord_Height; i++)
		{
			for (int j = 0; j < Coord_Width; j++)
			{
				if (Stage_Room_Coord[i][j].SpawnedComponents.Num() > 0)
				{
					for (int32 k = 0; k < Stage_Room_Coord[i][j].SpawnedComponents.Num(); k++)
					{
						ObjectPoolManager->ReturnStaticMeshComponent(Stage_Room_Coord[i][j].SpawnedComponents[k].Get<0>(), Stage_Room_Coord[i][j].SpawnedComponents[k].Get<1>());
					}
					Stage_Room_Coord[i][j].SpawnedComponents.Empty();
				}
				if (Stage_Room_Coord[i][j].Chest != nullptr)
				{
					ObjectPoolManager->ReturnChest(Stage_Room_Coord[i][j].Chest);
					Stage_Room_Coord[i][j].Chest = nullptr;
				}
			}
		}
	}
}

void ACGeneratedStage::Stage_CalculateDistance(FCoordinate& StartNode, bool bLightsCheck)
{
	if (bLightsCheck && Stage_Room_Coord[StartNode.Get<0>()][StartNode.Get<1>()].DistFromEntrance == 0) return;
	Stage_Room_Coord[StartNode.Get<0>()][StartNode.Get<1>()].DistFromEntrance = 0;
	int8 CurrentCheckingFlag = (Stage_Room_Coord[StartNode.Get<0>()][StartNode.Get<1>()].LastCheckedFlag + 1) % 127;
	Stage_Room_Coord[StartNode.Get<0>()][StartNode.Get<1>()].LastCheckedFlag = CurrentCheckingFlag;
	TQueue<FCoordinate> Q;
	Q.Enqueue(StartNode);
	if (Stage_Room_Coord[StartNode.Get<0>()][StartNode.Get<1>()].DebugTextComponent != nullptr)
	{
		Stage_Room_Coord[StartNode.Get<0>()][StartNode.Get<1>()].DebugTextComponent->SetText(
			FText::FromString(FString::FromInt(Stage_Room_Coord[StartNode.Get<0>()][StartNode.Get<1>()].DistFromEntrance))
		);
	}
	FVector tempMinus{ -1, -1, -1 };
	while (!Q.IsEmpty())
	{
		FCoordinate CurrentNode;
		Q.Dequeue(CurrentNode);

		for (const FCoordinate& dir : Directions)
		{
			FCoordinate NextNode = SumCoordinate(CurrentNode, dir);
			if (!IsCoordinateInBound(NextNode)) continue;
			bool bIsRoom = (
				(
					Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_EDGE ||
					(
						Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_STAIR ||
						Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_STAIR_DOOR
						)
					) ||
				(
					Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_BOSS_DOOR ||
					Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_BOSS
					)
				) ? true : false;
			if ((Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State != ROOM_CORRIDOR &&
					Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State != ROOM_OCCUPIED) && (!bIsRoom)) continue;
			if (Stage_Room_Coord[CurrentNode.Get<0>()][CurrentNode.Get<1>()].State == ROOM_CORRIDOR &&
				Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_STAIR) continue;
			if (Stage_Room_Coord[CurrentNode.Get<0>()][CurrentNode.Get<1>()].State == ROOM_STAIR &&
				(
					Stage_Room_Coord[CurrentNode.Get<0>()][CurrentNode.Get<1>()].State != ROOM_STAIR &&
					Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State != ROOM_STAIR_DOOR)) continue;
			if (Stage_Room_Coord[CurrentNode.Get<0>()][CurrentNode.Get<1>()].State == ROOM_CORRIDOR &&
				Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_BOSS) continue;
			if (Stage_Room_Coord[CurrentNode.Get<0>()][CurrentNode.Get<1>()].State == ROOM_BOSS &&
				Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_CORRIDOR) continue;
			if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].LastCheckedFlag == CurrentCheckingFlag) continue;
			Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].LastCheckedFlag = CurrentCheckingFlag;
			Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DistFromEntrance = Stage_Room_Coord[CurrentNode.Get<0>()][CurrentNode.Get<1>()].DistFromEntrance + 1;

			// Use Object Pooling -> Pool / Deactivate
			if (bLightsCheck)
			{
				bool bTurnOnLight = Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DistFromEntrance <= LightOnDistance ? true : false;

				// Spawn Chest To Occupied
				if (bTurnOnLight)
				{
					if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_OCCUPIED && Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest == nullptr)
					{
						int32 PlaceDirection = 0;
						int32 ChestDist = Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DistFromEntrance;
						for (int32 d = 0; d < 4; d++)
						{
							FCoordinate temp_dir = Directions[d];
							FCoordinate tempCoord = SumCoordinate(Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Coordinate, temp_dir);
							if (!IsCoordinateInBound(tempCoord)) continue;
							if (Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].State == ROOM_CORRIDOR &&
								Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].DistFromEntrance < ChestDist)
							{
								ChestDist = Stage_Room_Coord[tempCoord.Get<0>()][tempCoord.Get<1>()].DistFromEntrance;
								PlaceDirection = d;
							}
						}
						Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest = ObjectPoolManager->GetChest(
							this, FTransform(
								FRotator(0.f, -90.f * PlaceDirection, 0.f),
								FVector(NextNode.Get<0>() * 100.f * Stage_Scale, NextNode.Get<1>() * 100.f * Stage_Scale, 0.f)
							)
						);
						if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest != nullptr)
						{
							Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest->SetPlacedCoordinate(
								NextNode, this
							);
							Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest->ManualInteract(
								Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].bChestOpened ? INTERACTABLE_ITEM_STATE_OPEN_L : INTERACTABLE_ITEM_STATE_CLOSED
							);
							for (const int32 SpawnItemCode : Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].ChestSpawnItems)
							{
								if (SpawnItemCode < 0) continue;
								Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest->SpawnItemToStage(
									SpawnItemCode, ObjectPoolManager
								);
							}
						}
					}
				}
				else
				{
					if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_OCCUPIED && Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest != nullptr)
					{
						Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest->ReturnItemsFromStage(ObjectPoolManager, Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].ChestSpawnItems);
						ObjectPoolManager->ReturnChest(Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest);
 						Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Chest = nullptr;
					}
				}

				// Spawn Brazier To Boss Room
				if (bTurnOnLight)
				{
					if (NextNode == BossRoomCenterCoordinate && Spawned_Brazier == nullptr)
					{
						Spawned_Brazier = ObjectPoolManager->GetBrazier(this, FTransform(
							FVector(
								BossRoomCenterCoordinate.Get<0>() * 100.f * Stage_Scale,
								BossRoomCenterCoordinate.Get<1>() * 100.f * Stage_Scale,
								0.f
							)
						)
						);
					}
				}
				else
				{
					if (NextNode == BossRoomCenterCoordinate && Spawned_Brazier != nullptr)
					{
						ObjectPoolManager->ReturnBrazier(Spawned_Brazier);
						Spawned_Brazier = nullptr;
					}
				}

				// Pool / Return Door
				if (bTurnOnLight)
				{
					for (int32 idx_door = 0; idx_door < Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorTransformArr.Num(); idx_door++)
					{
						if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr[idx_door] == nullptr)
						{
							Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr[idx_door] = ObjectPoolManager->GetDoor(
								this,
								Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorTransformArr[idx_door]
							);
							if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorOpenStateArr.IsValidIndex(idx_door))
							{
								Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr[idx_door]->ManualInteract(
									Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorOpenStateArr[idx_door]
								);
							}
							Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr[idx_door]->SetPlacedCoordinate(NextNode, this);
							if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Generated_Room != nullptr)
							{
								Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Generated_Room->SetDoorPtr(Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr[idx_door]);
							}
						}
					}
				}
				else
				{
					for (int32 idx_door = 0; idx_door < Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr.Num(); idx_door++)
					{
						if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr[idx_door] != nullptr)
						{
							ObjectPoolManager->ReturnDoor(Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr[idx_door]);
							Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DoorArr[idx_door] = nullptr;
							if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Generated_Room != nullptr)
							{
								Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Generated_Room->SetDoorPtr(nullptr);
							}
						}
					}
				}

				// Pool / Return Light
				if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].State == ROOM_STAIR_DOOR &&
					Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Generated_Room != nullptr
					)
				{
					if (LightManager != nullptr) // Room Turn On Light
					{
						if (bTurnOnLight) Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Generated_Room->TurnOnLights(LightManager);
						else Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].Generated_Room->TurnOffLights(LightManager);
					}
				}
				else if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandlePosArr.Num() > 0) // Corridor Turn On Light
				{
					if (LightManager != nullptr)
					{
						for (int c = 0; c < Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandlePosArr.Num(); c++)
						{
							if (bTurnOnLight)
							{
								if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr.IsValidIndex(c) &&
									Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr[c] == nullptr)
								{
									UPointLightComponent* PLCa = LightManager->GetPointLightComponent(this);
									if (PLCa != nullptr)
									{
										PLCa->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
										PLCa->SetRelativeLocation(Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandlePosArr[c]);
										PLCa->Activate();
										//PLCa->SetLightBrightness((LightOnDistance - Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DistFromEntrance) / LightOnDistance + 0.2f);
										Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr[c] = PLCa;
									}
								}
							}
							else
							{
								if (Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr.IsValidIndex(c) &&
									Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr[c] != nullptr)
								{
									Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr[c]->Deactivate();
									Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr[c]->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
									LightManager->ReturnPointLightComponent(
										Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr[c]
									);
									Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].CandleLightArr[c] = nullptr;
								}
							}
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("ACGeneratedStage : Stage_CalculateDistance : Cannot Find Light Manager"));
					}
				}
			}
			if (bLightsCheck && Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DebugTextComponent != nullptr)
			{
				//Debug
				Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DebugTextComponent->SetText(
					FText::FromString(FString::FromInt(Stage_Room_Coord[NextNode.Get<0>()][NextNode.Get<1>()].DistFromEntrance))
				);
			}
			Q.Enqueue(NextNode);
		}
	}
}

UStaticMesh* ACGeneratedStage::SpawnWall(FVector Location, int32 Type, int32 Distance)
{
	UStaticMesh* SM = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/LevelPrototyping/Meshes/SM_Cube"));;
	UMaterialInterface* MI = nullptr;
	switch (Type)
	{
	case(ROOM_BLANK):
		break;
	case(ROOM_OCCUPIED):
		MI = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Green"));
		break;
	case(ROOM_CORRIDOR):
		MI = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Blue"));
		break;
	case(ROOM_EDGE):
		MI = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Yellow"));
		break;
	case(ROOM_STAIR):
		MI = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Green"));
		break;
	case(ROOM_STAIR_DOOR):
		MI = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Yellow"));
		break;
	case(ROOM_BOSS):
		MI = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Red"));
		break;
	case(ROOM_BOSS_DOOR):
		MI = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Yellow"));
		break;
	default:
		break;
	}
	//if (Type > ROOM_BLANK) Location.Z -= 100.f;
	Location += GetActorLocation();
	AStaticMeshActor* A = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, FRotator::ZeroRotator);
	UStaticMeshComponent* SMC = A->GetStaticMeshComponent();
	if (SMC != nullptr)
	{
		if (SM != nullptr) SMC->SetStaticMesh(SM);
		if (MI != nullptr) SMC->SetMaterial(0, MI);
		SMC->SetRelativeScale3D(FVector{ Stage_Scale, Stage_Scale, Stage_Scale });
	}

	UTextRenderComponent* TextRender = NewObject<UTextRenderComponent>(A);
	TextRender->RegisterComponent();
	TextRender->AttachToComponent(SMC, FAttachmentTransformRules::SnapToTargetIncludingScale);
	TextRender->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
	TextRender->SetRelativeRotation(FRotator(90.f, 0.f, 180.f));
	TextRender->SetRelativeLocation(FVector(50.f, 50.f, 110.f));
	TextRender->SetText(FText::FromString(FString::FromInt(Distance)));

	return SM;
}

void ACGeneratedStage::GridAddMesh(FTransform Transform, int32 Mesh_Type, int32 Type)
{
	IIStageGrid_Meshes* StageGrid_Meshes = Cast<IIStageGrid_Meshes>(Stage_Grid_Meshes[Mesh_Type]);
	if (StageGrid_Meshes == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ACGeneratedStage : GridAddMesh : Stage_Grid_Meshes Not Found"));
		return;
	}
	StageGrid_Meshes->AddGrid(Transform, Type);
}

void ACGeneratedStage::LightsOn()
{
	Stage_CalculateDistance(StartDoorCoordinate, true);
	//Delegate_LightOn.Unbind();
}

void ACGeneratedStage::UpdateMinimap(UCanvas* Canvas, int32 Width, int32 Height)
{
	bool IsMinimap = Canvas->SizeX < 800.f ? true : false;
	
	float MinimapTileSize = IsMinimap ? 15.f : 15.f;
	int32 CanvasX = Canvas->SizeX / (MinimapTileSize * 2);
	int32 CanvasY = Canvas->SizeY / (MinimapTileSize * 2);
	int32 PlayerX = CurrentPlayerCoordinate.Get<0>();
	int32 PlayerY = CurrentPlayerCoordinate.Get<1>();
	for (int32 i = PlayerX - CanvasX; i < PlayerX + CanvasX; i++)
	{
		for (int32 j = PlayerY - CanvasY; j < PlayerY + CanvasY; j++)
		{
			if ((i < 0 || j < 0) || (i >= Coord_Height || j >= Coord_Width))
			{
				int32 check_i = i;
				int32 check_j = j;
				if (i == -1) check_i++;
				else if (i == Coord_Height) check_i--;
				if (j == -1) check_j++;
				else if (j == Coord_Width) check_j--;
				if ((check_i < 0 || check_j < 0) || (check_i >= Coord_Height || check_j >= Coord_Width))
				{
					continue;
				}
				else if (Stage_Room_Coord[check_i][check_j].State != ROOM_BLANK && Stage_Room_Coord[check_i][check_j].ShowOnMinimap)
				{
					Canvas->DrawTile(
						MinimapTile_Blue, (i - (PlayerX - CanvasX)) * MinimapTileSize, (j - (PlayerY - CanvasY)) * MinimapTileSize, MinimapTileSize - 1.f, MinimapTileSize - 1.f, 0.f, 0.f, MinimapTileSize - 1.f, MinimapTileSize - 1.f, EBlendMode::BLEND_Opaque
					);
				}
				continue;
			}
			if (!Stage_Room_Coord[i][j].ShowOnMinimap) continue;
			if (i == PlayerX && j == PlayerY)
			{
			}
			else if (Stage_Room_Coord[i][j].State != ROOM_BLANK)
			{
				Canvas->DrawTile(
					MinimapTile, (i - (PlayerX - CanvasX)) * MinimapTileSize, (j - (PlayerY - CanvasY)) * MinimapTileSize, MinimapTileSize - 1.f, MinimapTileSize - 1.f, 0.f, 0.f, MinimapTileSize - 1.f, MinimapTileSize - 1.f, EBlendMode::BLEND_Opaque
				);
			}
			else if (Stage_Room_Coord[i][j].State != ROOM_EDGE)
			{
				Canvas->DrawTile(
					MinimapTile_Yellow, (i - (PlayerX - CanvasX)) * MinimapTileSize, (j - (PlayerY - CanvasY)) * MinimapTileSize, MinimapTileSize - 1.f, MinimapTileSize - 1.f, 0.f, 0.f, MinimapTileSize - 1.f, MinimapTileSize - 1.f, EBlendMode::BLEND_Opaque
				);
			}
			else
			{
				Canvas->DrawTile(
					MinimapTile_Blue, (i - (PlayerX - CanvasX)) * MinimapTileSize, (j - (PlayerY - CanvasY)) * MinimapTileSize, MinimapTileSize - 1.f, MinimapTileSize - 1.f, 0.f, 0.f, MinimapTileSize - 1.f, MinimapTileSize - 1.f, EBlendMode::BLEND_Opaque
				);
			}
		}
	}
}

void ACGeneratedStage::Room_Union(int32 x, int32 y)
{
	int32 RootX = Room_Find(x);
	int32 RootY = Room_Find(y);
	if (RootX == RootY) return;

	if (Stage_Rooms[RootX]->Root < Stage_Rooms[RootY]->Root)
	{
		Stage_Rooms[RootY]->Root = RootX;
	}
	else Stage_Rooms[RootX]->Root = RootY;
}

int32 ACGeneratedStage::Room_Find(int32 n)
{
	if (Stage_Rooms[n]->Root != n)
	{
		Stage_Rooms[n]->Root = Room_Find(Stage_Rooms[n]->Root);
	}
	return Stage_Rooms[n]->Root;
}

void ACGeneratedStage::MinimapRemoveBind()
{
	if (MinimapRenderTarget != nullptr)
	{
		MinimapRenderTarget->OnCanvasRenderTargetUpdate.RemoveDynamic(this, &ACGeneratedStage::UpdateMinimap);
	}
	if (WidemapRenderTarget != nullptr)
	{
		WidemapRenderTarget->OnCanvasRenderTargetUpdate.RemoveDynamic(this, &ACGeneratedStage::UpdateMinimap);
	}
}

void ACGeneratedStage::Entered_Stage(UObject* Player_Object, UObject* PlayerController_Object)
{
	IIPlayerOnStage* IPOS = Cast<IIPlayerOnStage>(Player_Object);
	if (IPOS != nullptr) IPlayerCharacter_Stage = IPOS;
	else UE_LOG(LogTemp, Error, TEXT("ACGeneratedStage : Entered_Stage : Player Cast Failed"));
	IIPlayerControllerStage* IPCS = Cast<IIPlayerControllerStage>(PlayerController_Object);
	IIPlayerControllerUI* IPCUI = Cast<IIPlayerControllerUI>(PlayerController_Object);
	if (IPCUI != nullptr) IPlayerController_UI = IPCUI;
	else UE_LOG(LogTemp, Error, TEXT("ACGeneratedStage : Entered_Stage : Player Controller Failed"));

	if (IPCS != nullptr && IPCUI != nullptr)
	{
		IPlayerController_Stage = IPCS;
		IPlayerController_Stage->MinimapRemoveBind();
		IPlayerController_Stage->SetStage(this);
		IIHUD* H = IPlayerController_UI->GetHUDInterface();
		IIHUD* W = IPlayerController_UI->GetWidemapInterface();
		if (H != nullptr)
		{
			MinimapRenderTarget = H->GetMinimapRT2D();
			MinimapRenderTarget->OnCanvasRenderTargetUpdate.AddDynamic(this, &ACGeneratedStage::UpdateMinimap);
		}
		if (W != nullptr)
		{
			WidemapRenderTarget = W->GetMinimapRT2D();
			WidemapRenderTarget->OnCanvasRenderTargetUpdate.AddDynamic(this, &ACGeneratedStage::UpdateMinimap);
		}
	}
	else UE_LOG(LogTemp, Error, TEXT("ACGeneratedStage : Entered_Stage : Player Controller Cast Failed"));
	if (!bCreatedStageCoordinates) Stage_GridGenerate();
	//Delegate_Entered_Stage.Unbind();
}

bool ACGeneratedStage::Room_FindClosestNode(FCoordinate Val, FCoordinate& rtn)
{
	TQueue<FCoordinate> Q;
	TSet<FCoordinate> Visited;
	Q.Enqueue(Val);
	Visited.Add(Val);

	while (!Q.IsEmpty())
	{
		FCoordinate Curr;
		Q.Dequeue(Curr);

		for (const FCoordinate& d : Directions)
		{
			FCoordinate Node = FCoordinate{
				Curr.Get<0>() + d.Get<0>(),
				Curr.Get<1>() + d.Get<1>(),
				Curr.Get<2>() + d.Get<2>(),
			};
			int32 x = Curr.Get<0>();
			int32 y = Curr.Get<1>();

			if ((x < 0 || y < 0) || (x >= Coord_Height || y >= Coord_Width)) continue;
			if (Stage_Room_Coord[x][y].State == ROOM_BLANK) continue;
			if (Visited.Contains(Node)) continue;

			if (Stage_Room_Coord[x][y].State == ROOM_OCCUPIED)
			{
				rtn.Get<0>() = x;
				rtn.Get<1>() = y;
				return true;
			}
			Visited.Add(Node);
			Q.Enqueue(Node);
		}
	}
	return false;
}

int32 ACGeneratedStage::Room_GetManhattan(FCoordinate A, FCoordinate B)
{
	return FMath::Abs(A.Get<0>() - B.Get<0>()) + FMath::Abs(A.Get<1>() - B.Get<1>()) + FMath::Abs(A.Get<2>() - B.Get<2>());
}

void ACGeneratedStage::LightsOut()
{
	if (LightManager)
	{
		IIStageGrid_Meshes* ISGM = Cast<IIStageGrid_Meshes>(Stage_Grid_Meshes[STAGE_GRID_MESH_STAIR]);
		if (ISGM != nullptr) ISGM->TurnOffLights(LightManager);
	}

	for (int32 i = 0; i < Coord_Height; i++)
	{
		for (int32 j = 0; j < Coord_Width; j++)
		{
			if (Stage_Room_Coord[i][j].CandleLightArr.Num() == 0) continue;
			for (int32 k = 0; k < Stage_Room_Coord[i][j].CandleLightArr.Num(); k++)
			{
				UPointLightComponent* PLC = Stage_Room_Coord[i][j].CandleLightArr[k];
				if (PLC == nullptr) continue;
				if (LightManager != nullptr) LightManager->ReturnPointLightComponent(PLC);
				Stage_Room_Coord[i][j].CandleLightArr[k] = nullptr;
			}
		}
	}
	IPlayerCharacter_Stage = nullptr;
	IPlayerController_Stage = nullptr;
	MinimapRenderTarget = nullptr;
}

void ACGeneratedStage::PlayerSightReached(FVector& SightLocation)
{
	FCoordinate SightCoord{
		FMath::FloorToInt32((SightLocation.X + 200.f) / 400.f),
		FMath::FloorToInt32((SightLocation.Y + 200.f) / 400.f),
		0
	};

	TQueue<FCoordinate> Q;
	Q.Enqueue(CurrentPlayerCoordinate);
	while (!Q.IsEmpty())
	{
		FCoordinate CurrCoord;
		Q.Dequeue(CurrCoord);
		int32 MHTDist = Room_GetManhattan(CurrCoord, SightCoord);
		
		for (const FCoordinate& dir : Directions)
		{
			FCoordinate NextCoord = SumCoordinate(CurrCoord, dir);
			int32 NextMHTDist = Room_GetManhattan(NextCoord, SightCoord);
			if (!IsCoordinateInBound(NextCoord)) continue;
			//if (
			//	Stage_Room_Coord[NextCoord.Get<0>()][NextCoord.Get<1>()].State == ROOM_BLANK ||
			//	Stage_Room_Coord[NextCoord.Get<0>()][NextCoord.Get<1>()].State == Stage_Room_Coord[CurrCoord.Get<0>()][CurrCoord.Get<1>()].State)
			//{
			//	Stage_Room_Coord[NextCoord.Get<0>()][NextCoord.Get<1>()].ShowOnMinimap = true;
			//}
			Stage_Room_Coord[NextCoord.Get<0>()][NextCoord.Get<1>()].ShowOnMinimap = true;
			if (NextMHTDist < MHTDist)
			{
				//Stage_Room_Coord[NextCoord.Get<0>()][NextCoord.Get<1>()].ShowOnMinimap = true;
				Q.Enqueue(NextCoord);
			}
		}
	}
}