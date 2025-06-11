#include "CGameMode_Stage.h"
#include "Engine/StaticMeshActor.h"
#include "Dungeon/CStageGrid_Corridor.h"
#include "Interfaces/IStageGrid_Meshes.h"
#include "Dungeon/CGeneratedStage.h"
#include "GameSystem/CGameMode_Stage.h"
#include "GameSystem/CGameState_Stage.h"
#include "Player/CPlayerController.h"
#include "PCH.h"

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