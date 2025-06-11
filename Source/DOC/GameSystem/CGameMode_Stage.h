#pragma once

#include "CoreMinimal.h"
#include "GameSystem/DOCGameMode.h"
#include "PCH.h"
#include "Interfaces/IGameModeDataManager.h"
#include "CGameMode_Stage.generated.h"

using FCoordinate = TTuple<int32, int32, int32>;

UCLASS()
class DOC_API ACGameMode_Stage : public ADOCGameMode
{
	GENERATED_BODY()
	
	ACGameMode_Stage();
public:
	//virtual void Debug_GenerateMap() override;

	//virtual void RegisterCharacterToStage(class ACharacter* PlayerCharacter, int32 floor) override;
	//virtual void RegisterMinimapRenderTargetToStage(class UCanvasRenderTarget2D* MinimapRenderTarget, int32 floor) override;
private:
	TArray<class ACGeneratedStage*> Stages;
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;
	virtual void BeginPlay() override;

	// Data Asset

};
