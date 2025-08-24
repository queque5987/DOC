#include "CGameMode_Stage.h"
#include "Engine/StaticMeshActor.h"
#include "Dungeon/CStageGrid_Corridor.h"
#include "Interfaces/IStageGrid_Meshes.h"
#include "Dungeon/CGeneratedStage.h"
#include "GameSystem/CGameMode_Stage.h"
#include "GameSystem/CGameState_Stage.h"
#include "Player/CPlayerController.h"
#include "Player/UI/CSpawnedEnemyData.h"

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
	StageClearedDelegate.AddUFunction(this, TEXT("StageClearedCallback"));
}

void ACGameMode_Stage::StageClearedCallback(UObject* PlayerCharacter, const TArray<class UCSpawnedEnemyData*>& ClearedItems)
{
	if (GEngine)
	{
		if (IsValid(PlayerCharacter))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("StageClearedCallback: PlayerCharacter is valid."));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("StageClearedCallback: PlayerCharacter is NOT valid."));
		}

		for (const auto& Item : ClearedItems)
		{
			if (IsValid(Item))
			{
				FString ItemNameStr = Item->GetName();
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Cleared Item: %s"), *ItemNameStr));
			}
		}
	}
}
