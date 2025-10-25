#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "Json.h"
#include "Interfaces/CStageStructs.h"
#include "CHttpComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOC_API UCHttpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCHttpComponent();

protected:
	virtual void BeginPlay() override;
	//FPlayerTimeSeriesData PlayerTimeSeriesData;
	//FPlayerTimeSeriesDataV2 PlayerTimeSeriesDataV2;
	//TArray<TArray<FPlayerTimeSeriesDataV3>> PlayerTimeSeriesDataV3_Container;
public:	
	FHttpModule* Http;
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//void SendRequest();
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//void AddTimeSeriesData(int32 CurrButton, float CurrRelativeDist, float DistFromTop, float DistFromBot, float DistFromLeft, float DistFromRight, float PlayerHP, float PlayerStamina);
	//void AddTimeSeriesData(
	//	FVector PlayerForwardVector,
	//	float PlayerVelocity, 
	//	FVector PlayerMovingDirectionVector,
	//	FVector RelativeDirectionVector, 
	//	float RelativeDistance, 
	//	float DistFromTop, float DistFromBot, 
	//	float DistFromLeft, float DistFromRight, 
	//	float PlayerHP, float PlayerStamina
	//);
	//void AddTimeSeriesData(TDoubleLinkedList<FPlayerTimeSeriesDataV3>* TimeSerieseDataLL);
	void Post_TimeSeriesData(TDoubleLinkedList<FPlayerTimeSeriesDataV3>* TimeSerieseDataLL);
};