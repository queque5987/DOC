#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerControllerUI.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CPlayerController.generated.h"

UCLASS()
class DOC_API ACPlayerController : public APlayerController, public IIPlayerControllerStage, public IIPlayerControllerUI
{
	GENERATED_BODY()
	
	ACPlayerController();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> HUDClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> InventoryClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> WidemapClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> ItemTooltipWidgetClass;

	UPROPERTY()
	class UCHUD* Widget_HUD;
	UPROPERTY()
	class UCInventory* Widget_Inventory;
	UPROPERTY()
	class UCWidemap* Widget_Widemap;
	UPROPERTY()
	class UCItemTooltipWidget* Widget_ItemTooltip;

	FOnItemHovered OnItemHoveredDelegate;
	FOnItemUnhovered OnItemUnhoveredDelegate;

	class ACStatusStage* StatusStage;

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
	virtual bool AttachEquipment(class IIEquipment* ToEquipItem, int32 Type, FName SocketName) override;

	virtual void LockOnMonster(class IIEnemyCharacter* Enemy) override;
	virtual void LockFreeMonster() override;
	//virtual void SetupDelegates(FMONTAGE_PLAYING_STATE_CHANGED* Delegate_MontagePlayingStateChanged) override;
	/*
		UI
	*/

	virtual class IIHUD* GetWidemapInterface() override;
	virtual class IIHUD* GetHUDInterface() override;
	virtual void ToggleInventory() override;
	virtual bool InsertItem(FINSERT_ITEM*& Delegate_InsertItem, AActor* Item, int32 ItemType) override;
	virtual bool InsertItem(class UCItemData* ItemData, class AActor* Item) override;
	//virtual bool InsertEquipment(class UCItemData* ItemData, class AActor* Equipment, int32 EquipmentType) override;
	/* Called When Add Item To Inventory */
	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) override;
	virtual void ToggleWidemap(bool e) override;
	virtual void ToggleMinimap(bool e) override;
	virtual bool IsInventoryVisible() override;

	UFUNCTION()
	virtual void ShowItemTooltip(class UCItemData* ItemData) override;

	UFUNCTION()
	virtual void HideItemTooltip() override;
};
