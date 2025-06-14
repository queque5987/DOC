#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerControllerUI.h"
#include "CPlayerController.generated.h"

UCLASS()
class DOC_API ACPlayerController : public APlayerController, public IIPlayerControllerStage, public IIPlayerControllerUI
{
	GENERATED_BODY()
	
	ACPlayerController();

	TSubclassOf<class UUserWidget> HUDClass;
	TSubclassOf<class UUserWidget> InventoryClass;
	TSubclassOf<class UUserWidget> WidemapClass;
	class UCHUD* Widget_HUD;
	class UCInventory* Widget_Inventory;
	class UCWidemap* Widget_Widemap;

	class IIGeneratedStage* CurrentStage;
	class IIGeneratedStage* PreviousStage;
	class IIGameModeDataManager* GameModeDataManager;
	class IIObjectPoolManager* ObjectPoolManager;
	class IIPlayerState* PlayerState;
	class IIPlayerOnStage* PlayerCharacterStage;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:

public:
	/*
		Stage Controll
	*/

	virtual void MinimapRemoveBind() override;
	virtual void LightsOutPrevStage() override;
	/*Set CurrentStage To PreviousStage*/
	virtual void SetStage(class IIGeneratedStage* Stage) override;
	virtual void SightReached(FVector& SightLocation) override;
	virtual FVector GetPlayerLocation() override;
	virtual void SetToPerspectiveCamera(FTransform Transform) override;
	virtual void SetToFollowCamera() override;
	virtual void GetUnderCursor(FHitResult& HitResult) override;

	virtual bool RecieveDamage(FDamageConfig DamageConfig) override;
	/*
		UI
	*/

	virtual class IIHUD* GetWidemapInterface() override;
	virtual class IIHUD* GetHUDInterface() override;
	virtual void ToggleInventory() override;
	virtual void InsertItem(FINSERT_ITEM*& Delegate_InsertItem, AActor* Item, int32 ItemType) override;
	/* Called When Add Item To Inventory */
	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) override;
	virtual void ToggleWidemap(bool e) override;
	virtual void ToggleMinimap(bool e) override;
};
