#pragma once

#include "CoreMinimal.h"
#include "CStageGrid_Corridor.h"
#include "CStageDelegateTypes.h"
#include "CGeneratedStair.generated.h"

UCLASS()
class DOC_API ACGeneratedStair : public ACStageGrid_Corridor
{
	GENERATED_BODY()

	ACGeneratedStair();
protected:
	TArray<FVector> CandlePosArr;
	TArray<class UPointLightComponent*> CandleArr;
	UPROPERTY(EditAnywhere)
	class USphereComponent* EntranceCollider;
	UPROPERTY(EditAnywhere)
	class USphereComponent* ExitCollider;
	
	FLIGHTS_ON* Delegate_LightOn;
	FENTERED_STAGE* Delegate_Entered_Stage;
	class IIObjectPoolManager* ObjectPoolManager;
	UFUNCTION()
	void OnEntranceBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnExitBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	virtual void GenerateStair(bool bIsEntrance, FLIGHTS_ON* DeleLightOn, FENTERED_STAGE* DeleEnteredStage) override;
	virtual void BeginPlay() override;
	virtual void TurnOnLights(class IIGameStateLightManager* LightManager) override;
	virtual void TurnOffLights(class IIGameStateLightManager* LightManager) override;
};
