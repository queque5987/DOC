#include "CGameMode_Stage.h"
#include "Engine/StaticMeshActor.h"
#include "CStageGrid_Corridor.h"
#include "IStageGrid_Meshes.h"
#include "CGeneratedStage.h"
#include "CGameState_Stage.h"
#include "CPlayerController.h"
#include "PCH.h"

//const int32 ACGameMode_Stage::ROOM_BLANK = 0;
//const int32 ACGameMode_Stage::ROOM_OCCUPIED = 1;
//const int32 ACGameMode_Stage::ROOM_CORRIDOR = 2;
//const int32 ACGameMode_Stage::ROOM_TELEPORT = 3;
//const int32 ACGameMode_Stage::ROOM_BOSS = 4;
//const int32 ACGameMode_Stage::ROOM_EDGE = 5;

ACGameMode_Stage::ACGameMode_Stage() : Super()
{
	GameStateClass = ACGameState_Stage::StaticClass();
	PlayerControllerClass = ACPlayerController::StaticClass();
}

void ACGameMode_Stage::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ACGameMode_Stage::StartPlay()
{
	Super::StartPlay();
}

void ACGameMode_Stage::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetWorldSettings()->KillZ = TNumericLimits<float>::Lowest();
}