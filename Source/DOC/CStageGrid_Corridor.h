#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PCH.h"
#include "IStageGrid_Meshes.h"
#include "CStageGrid_Corridor.generated.h"

UCLASS()
class DOC_API ACStageGrid_Corridor : public AActor, public IIStageGrid_Meshes
{
	GENERATED_BODY()
	
public:	
	ACStageGrid_Corridor();
protected:
	TArray<class UHierarchicalInstancedStaticMeshComponent*> HISM_Arr;

	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Floor1;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Floor2;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Floor3;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Floor4;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Floor5;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Wall;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Wall_Base;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Wall_Clamp1;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Wall_Clamp1_Base;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Wall_Door;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Wall_Door_Base;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Philar1;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Philar2;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Ceiling1;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Ceiling2;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Ceiling3;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Ceiling_Clamp;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_CeilingArchWall;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_CeilingArch;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Candle;
	UPROPERTY(EditAnywhere)
	class UHierarchicalInstancedStaticMeshComponent* HISM_Candlelabra;
	//UPROPERTY(EditAnywhere)
	//class UHierarchicalInstancedStaticMeshComponent* HISM_CandleFlame;

	class UStaticMesh* SM_Floor1;
	class UStaticMesh* SM_Floor2;
	class UStaticMesh* SM_Floor3;
	class UStaticMesh* SM_Floor4;
	class UStaticMesh* SM_Floor5;
	class UStaticMesh* SM_Wall;
	class UStaticMesh* SM_Wall_Base;
	class UStaticMesh* SM_Wall_Clamp1;
	class UStaticMesh* SM_Wall_Clamp1_Base;
	class UStaticMesh* SM_Wall_Door;
	class UStaticMesh* SM_Wall_Door_Base;
	class UStaticMesh* SM_Philar1;
	class UStaticMesh* SM_Philar2;
	class UStaticMesh* SM_Ceiling1;
	class UStaticMesh* SM_Ceiling2;
	class UStaticMesh* SM_Ceiling3;
	class UStaticMesh* SM_CeilingArchWall;
	class UStaticMesh* SM_CeilingArch;
	class UStaticMesh* SM_Candle;
	class UStaticMesh* SM_Candlelabra;
	//class UStaticMesh* SM_CandleFlame;


protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void AddGrid(FTransform Transform, int32 Type, bool bWorldSpace = false) override;
	virtual void ClearGrid() override;
};
