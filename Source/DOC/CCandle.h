#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCandle.generated.h"

UCLASS()
class DOC_API ACCandle : public AActor
{
	GENERATED_BODY()
	
public:	
	ACCandle();

protected:
	class UStaticMeshComponent* StaticMeshComponent_Candle;
	class UStaticMeshComponent* StaticMeshComponent_Candlelabra;
	class UStaticMeshComponent* StaticMeshComponent_Candle_Flame;

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
