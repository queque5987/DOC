#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IInteractableItem.h"
#include "PCH.h"
#include "CBrazier.generated.h"

UCLASS()
class DOC_API ACBrazier : public AActor, public IIInteractableItem
{
	GENERATED_BODY()
	
public:	
	ACBrazier();

protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SMC_Brazier_Top;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SMC_Brazier_Body;
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* NC_Brazier_FX;
	UPROPERTY(EditAnywhere)
	class UPointLightComponent* PLC_Brazier_PointLight;
	bool Selected = false;
	bool bBusy = false;
	bool TurnOn = false;
	float PLIntensity = 0.f;
	float MaxPLIntensity = 16.f;
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) override;
	//virtual bool GetBusy() override { return bBusy; };
	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	virtual void Select() override;
	virtual void UnSelect() override;
	virtual void SetVisibility(bool e) override;
	virtual bool GetVisibility() override;

	//virtual void ManualInteract(int32 OpenState) override;
	//virtual void SetPlacedCoordinate(TTuple<int32, int32, int32> Coordinate, class IIGeneratedStage* Stage, int32 idx = 0) override {
	//	PlacedCoordinate = Coordinate;
	//	PlacedStage = Stage;
	//};
	//virtual TTuple<int32, int32, int32> GetPlacedCoordinate() override { return PlacedCoordinate; };
};
