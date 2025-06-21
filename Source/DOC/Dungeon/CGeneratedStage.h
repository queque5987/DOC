#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IStageBuild.h"
#include "PCH.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Interfaces/IGeneratedStage.h"
#include "CGeneratedStage.generated.h"


using FCoordinate = TTuple<int32, int32, int32>;
using FSpawnedStaticMeshComponent = TTuple<class UStaticMeshComponent*, int32>;
UCLASS()
class DOC_API ACGeneratedStage : public AActor, public IIGeneratedStage
{
	GENERATED_BODY()
	
public:	
	ACGeneratedStage();

	FLIGHTS_ON Delegate_LightOn;
	FENTERED_STAGE Delegate_Entered_Stage;

	virtual void SetStairCoord(int32 Coord_H, int32 Coord_W, int32 Direction, int32& rtn_Coord_H, int32& rtn_Coord_W, int32& rtn_Direction, bool bIsEntrance = false) override;
private:
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneComponent;

	class IIPlayerOnStage* IPlayerCharacter_Stage = nullptr;
	class IIPlayerControllerStage* IPlayerController_Stage = nullptr;
	class IIPlayerControllerUI* IPlayerController_UI = nullptr;
	class UCanvasRenderTarget2D* MinimapRenderTarget = nullptr;
	class UCanvasRenderTarget2D* WidemapRenderTarget = nullptr;

	TArray<class AActor*> Stage_Grid_Meshes;
	int32 Coord_Width;
	int32 Coord_Height;
	int32 Max_Corridor_Length;
	float Room_Ratio;
	float Room_Distance;
	float Clamp_Ratio;
	float Clamp_Criteria_Ratio;
	int32 Max_Clamp_Level;
	int32 LightOnDistance = 9;
	const float Stage_Scale = 4.f;
	bool bCreateBossRoom = true;
	bool bCreateStair;
	bool bCreatedStageCoordinates = false;
	FCoordinate StairFromTopCoordinate;
	FCoordinate StairToBottomCoordinate;
	FCoordinate StartDoorCoordinate;
	FCoordinate CurrentPlayerCoordinate;
	FCoordinate BossRoomCenterCoordinate;
	class IIInteractableItem* Spawned_Brazier = nullptr;

	class UTexture* MinimapTile;
	class UTexture* MinimapTile_Blue;
	class UTexture* MinimapTile_Yellow;

	struct FRoom_Info
	{
		FRoom_Info(FCoordinate _Coordinate, int32 _Root, int32 _State, int32 _Index) :
			Coordinate(_Coordinate), Root(_Root), State(_State), Index(_Index)
		{
			Floor = 0;
			DistFromEntrance = 0;
			Generated_Room = nullptr;
			DebugTextComponent = nullptr;
			LastCheckedFlag = 0;
			ShowOnMinimap = false;
			IsGenerated = false;
			Chest = nullptr;
		};

		FCoordinate Coordinate;
		int32 Root;
		int32 State;
		int32 Index;
		int32 Floor;
		int32 DistFromEntrance;
		int8 LastCheckedFlag;
		bool ShowOnMinimap;
		bool IsGenerated;
		bool bChestOpened;
		class IIInteractableItem* Chest;
		TArray<class IIInteractableItem*> DoorArr;
		TArray<int32> DoorOpenStateArr;
		TArray<FTransform> DoorTransformArr;
		TArray<FVector> CandlePosArr;
		TArray<class UPointLightComponent*> CandleLightArr;
		TArray<FSpawnedStaticMeshComponent> SpawnedComponents;
		TArray<int32> ChestSpawnItems;
		class UPointLightComponent* Candle_a;
		class UPointLightComponent* Candle_b;
		class IIStageGrid_Room* Generated_Room;
		class UTextRenderComponent* DebugTextComponent;
	};

	TArray<TArray<FRoom_Info>> Stage_Room_Coord;
	TArray<struct FRoom_Info*> Stage_Rooms;

	class IIGameStateLightManager* LightManager;
	class IIObjectPoolManager* ObjectPoolManager;
public:
	//void SetPlayer(class ACharacter* C) { PlayerCharacter = C; };
	/*Deprecated*/
	//void SetMinimapRenderTarget(class UCanvasRenderTarget2D* T);
	int32 GetCoord_Widght() { return Coord_Width; }
	int32 GetCoord_Height() { return Coord_Height; }
	void SetCoord_Widght(int32 e) { Coord_Width = e; }
	void SetCoord_Height(int32 e) { Coord_Height = e; }

	static const int32 ROOM_BLANK;
	static const int32 ROOM_OCCUPIED;
	static const int32 ROOM_CORRIDOR;
	static const int32 ROOM_STAIR;
	//static const int32 ROOM_STAIR_ENTRANCE;
	static const int32 ROOM_BOSS;
	static const int32 ROOM_BOSS_DOOR;
	static const int32 ROOM_EDGE;
	static const int32 ROOM_EDGE_DOOR;
	//static const int32 ROOM_STAIR;
	static const int32 ROOM_STAIR_DOOR;
	/*Degug Only*/
	static const int32 ROOM_DEBUG_BLOCK;

	const TArray<FCoordinate> Directions = {
		FCoordinate{0, 1, 0},
		FCoordinate{1, 0, 0},
		FCoordinate{0, -1, 0},
		FCoordinate{-1, 0, 0}
	};
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void GenerateStage() override;
	virtual void ClearStage() override;
	virtual void MinimapRemoveBind() override;
	virtual void LightsOut() override;
	virtual void PlayerSightReached(FVector& SightLocation) override;
	virtual void SetChestOpenState(TTuple<int32, int32, int32> Coordinate, int32 OpenState) override {
		Stage_Room_Coord[Coordinate.Get<0>()][Coordinate.Get<1>()].bChestOpened = (OpenState == INTERACTABLE_ITEM_STATE_CLOSED ? false : true);
	};
	virtual void SetDoorOpenState(TTuple<int32, int32, int32> Coordinate, int32 OpenState, int32 index) override {
		if (!Stage_Room_Coord[Coordinate.Get<0>()][Coordinate.Get<1>()].DoorOpenStateArr.IsValidIndex(index)) return;
		Stage_Room_Coord[Coordinate.Get<0>()][Coordinate.Get<1>()].DoorOpenStateArr[index] = OpenState;
	};

	UFUNCTION()
	void Entered_Stage(class UObject* Player_Object, class UObject* PlayerController_Object);
	UFUNCTION()
	void LightsOn();
	UFUNCTION()
	void UpdateMinimap(class UCanvas* Canvas, int32 Width, int32 Height);

	virtual void Stage_GridGenerate() override;
	virtual void Stage_GridReturn() override;
protected:
	void Stage_GridGenerate_Frag(int32 Height_m, int32 Height_M, int32 Width_m, int32 Width_M);
	class UStaticMesh* SpawnWall(FVector Location, int32 Type, int32 Distance = 0);
	void GridAddMesh(FTransform Transform, int32 Mesh_Type, int32 Type);
private:
	void Room_Union(int32 x, int32 y);
	int32 Room_Find(int32 n);
	bool Room_FindClosestNode(FCoordinate Val, FCoordinate& rtn);
	int32 Room_GetManhattan(FCoordinate A, FCoordinate B);

	bool Stage_CalculateClosestCorridor(TArray<FCoordinate>& Trail);
	bool Stage_FindTwoClosestPointsBetweenTwoNodes(int32 RootA, int32 RootB, FCoordinate& outA, FCoordinate& outB);
	bool Stage_CalculateCorridorBetweenTwoNodes(TArray<FCoordinate>& Trail, FCoordinate Start, FCoordinate Destination);
	void Stage_CalculateDistance(FCoordinate& StartNode, bool bLightsCheck = false);

	FCoordinate SumCoordinate(FCoordinate A, FCoordinate B);
	bool IsCoordinateInBound(FCoordinate A);
};
