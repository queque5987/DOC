#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractableItem.h"
#include "PCH.h"
#include "CDoor.generated.h"

UCLASS()
class DOC_API ACDoor : public AActor, public IIInteractableItem
{
	GENERATED_BODY()
	
public:	
	ACDoor();

protected:
	class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* SMC_DoorStamp;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* SMC_Door;

	TTuple<int32, int32, int32> PlacedCoordinate;
	class IIGeneratedStage* PlacedStage = nullptr;
	int32 PlacedIndex;
	bool Selected = false;
	bool bBusy = false;
	float Angle = 0.f;
	float TargetAngle = 0.f;
	float LMaxAngle = 90.f;
	float RMaxAngle = -90.f;
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	virtual void Select() override;
	virtual void UnSelect() override;
	virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) override;
	virtual void SetVisibility(bool e) override;
	virtual bool GetVisibility() override;
	virtual void ManualInteract(int32 OpenState) override;
	virtual void SetPlacedCoordinate(TTuple<int32, int32, int32> Coordinate, class IIGeneratedStage* Stage, int32 idx = 0) override {
		PlacedCoordinate = Coordinate; 
		PlacedStage = Stage;
		PlacedIndex = idx;
	};
	virtual TTuple<int32, int32, int32> GetPlacedCoordinate() override { return PlacedCoordinate; };
};
