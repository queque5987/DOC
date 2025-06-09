#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PCH.h"
#include "CStageGrid_Corridor.h"
#include "IStageGrid_Room.h"
#include "CGeneratedRoom.generated.h"

UCLASS()
class DOC_API ACGeneratedRoom : public ACStageGrid_Corridor, public IIStageGrid_Room
{
	GENERATED_BODY()
	
public:	
	ACGeneratedRoom();

protected:
	virtual void BeginPlay() override;
	bool bHasDoor = false;
public:	
	virtual void Tick(float DeltaTime) override;
	void Generate(int32 GridScale, float StageScale = 4.f);
	virtual bool GetHasDoor() override { return bHasDoor; };
	virtual void SetHasDoor(bool b) override { bHasDoor = b; };
};
