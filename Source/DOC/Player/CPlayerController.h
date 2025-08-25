#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerControllerUI.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "PCH.h"
#include "CPlayerController.generated.h"

class UCSpawnedEnemyData;

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
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> ChestItemWidgetClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> StageClearItemWidgetClass;

	UPROPERTY()
	class UCHUD* Widget_HUD;
	UPROPERTY()
	class UCInventory* Widget_Inventory;
	UPROPERTY()
	class UCWidemap* Widget_Widemap;
	UPROPERTY()
	class UCItemTooltipWidget* Widget_ItemTooltip;
	UPROPERTY()
	class UCItemTooltipWidget* Widget_ItemTooltip_Additional;
	UPROPERTY()
	class UCChestItemWidget* Widget_ChestItem;
	UPROPERTY()
	class UCStageClearItemWidget* Widget_StageClearItem;

	FOnItemHovered OnItemHoveredDelegate;
	FOnItemUnhovered OnItemUnhoveredDelegate;
	FEQUIP_ITEM Delegate_EquipItem;
	FUNEQUIP_ITEM Delegate_UnEquipItem;
	FOnChangeCounterReady OnChangeCounterReady;
	FOutOfMana Delegate_OutOfMana;
	FStageCleared* StageClearedDelegatePtr;
	FPressedKeyboard* Delegate_PressedKeyboardPtr;

	class ACStatusStage* StatusStage;

	class IIGeneratedStage* CurrentStage;
	class IIGeneratedStage* PreviousStage;
	//class IIGameModeDataManager* GameModeDataManager;
	class IIObjectPoolManager* ObjectPoolManager;
	class IIPlayerState* PlayerState;
	class IIPlayerOnStage* PlayerCharacterStage;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	TQueue<class UCDamage*> DamageComponentQueue;
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
	UFUNCTION()
	virtual bool RecieveDamage(FDamageConfig DamageConfig) override;
	virtual bool DealtDamage(FDamageConfig DamageConfig) override;
	virtual bool AttachEquipment(class IIEquipment* ToEquipItem, int32 Type, FName SocketName) override;
	virtual class IIObjectPoolManager* GetObjectPoolManager() override;
	virtual FRotator GetCurrentCameraRotation() override;
	virtual void LockOnMonster(class IIEnemyCharacter* Enemy) override;
	virtual void LockFreeMonster() override;
	virtual FPlayerStat GetPlayerStat() override;
	virtual FOnChangeCounterReady* GetOnChangeCounterReadyDelegate() override;
	virtual FOutOfMana* GetOutOfManaDelegate() override { return &Delegate_OutOfMana; };
	virtual bool TrySpendMP(float e) override;
	virtual float GetCurrentMP() override;
	virtual void SetupDelegates(FOnReceivedDamage* Delegate_OnReceivedDamage, FOnQuickSlotInput* Delegate_OnQuickSlotInput, FPressedKeyboard* Delegate_PressedKeyboard) override;
	//virtual void SetupDelegates(FMONTAGE_PLAYING_STATE_CHANGED* Delegate_MontagePlayingStateChanged) override;
	/*
		UI
	*/
	virtual void OpenChestItemWidget(TArray<class UCItemData*>* ToShowItemData) override;
	virtual void CloseChestItemWidget() override;
	virtual class IIHUD* GetWidemapInterface() override;
	virtual class IIHUD* GetHUDInterface() override;
	virtual void ToggleInventory() override;
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

	UFUNCTION()
	virtual void EquipItem(class UCItemData* ItemData) override;

	UFUNCTION()
	virtual void UnEquipItem(class UCItemData* ItemData) override;
	//virtual bool HasWeapon() override;
	virtual bool GetHasWeapon() override;
	virtual void SetHasWeapon(bool bHasWeapon) override;

	virtual void SetCounterHitCheck(bool b) override;
	virtual bool GetCounterHitCheck() override;
	UFUNCTION()
	void UseQuickslotItem(int32 QuickslotIndex);

	UFUNCTION()
	void OnStageCleared(class UObject* PlayerCharacter, const TArray<class UCSpawnedEnemyData*>& ClearedItems);
	UFUNCTION()
	void OnPressedKeyboard(FKey Key);
private:
	void EquipItem_Equipment(class UCItemData* ItemData);
	void EquipItem_Disposable(class UCItemData* ItemData);

	bool bCounterHitCheck = false;
};
