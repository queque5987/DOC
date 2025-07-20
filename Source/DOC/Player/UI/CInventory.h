#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "PCH.h"
#include "CInventory.generated.h"

// Forward Declarations
class UButton;
class UTextBlock;
class UTileView;
class UPanelWidget;

UCLASS()
class DOC_API UCInventory : public UUserWidget
{
	GENERATED_BODY()
public:
	struct FInventoryTabInfo
	{
		UButton* TabButton = nullptr;
		UTextBlock* TabText = nullptr;
		UTileView* TabTileView = nullptr;
		int32 Category = 0;
	};

	struct FStatusTabInfo
	{
		UButton* TabButton = nullptr;
		UTextBlock* TabText = nullptr;
		UPanelWidget* StatusPanel = nullptr;
		int32 Category = 0;
	};

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	bool Disabled = false;
	float DisableSec = 0.85f;
	FTimerHandle DisableTimer;
	FINSERT_ITEM Delegate_InsertItem;

	UPROPERTY(meta = (BindWidget))
	class UTileView* ItemTile;
	UPROPERTY(meta = (BindWidget))
	class UTileView* EquipmentTile;

	UPROPERTY(meta = (BindWidget))
	class UTileView* WeaponTile;
	UPROPERTY(meta = (BindWidget))
	class UTileView* HelmetTile;
	UPROPERTY(meta = (BindWidget))
	class UTileView* GloveTile;
	UPROPERTY(meta = (BindWidget))
	class UTileView* ShoseTile;
	UPROPERTY(meta = (BindWidget))
	class UTileView* TorsoTile;
	UPROPERTY(meta = (BindWidget))
	class UTileView* PantsTile;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Equipment;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Desposable;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Equiped;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Stat;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Btn_Equipment;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Btn_Desposable;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Btn_Equiped;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Btn_Stat;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* EquippedPanel;
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* StatPanel;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Equiped_Stat_Attack_Power;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Equiped_Stat_Defense_Power;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Equiped_Stat_Health_Regen_Power;

	UPROPERTY(EditAnywhere, Category = "UI|Tab Styling")
	TArray<FLinearColor> ActiveButtonColorArr;
	
	UPROPERTY(EditAnywhere, Category = "UI|Tab Styling")
	FLinearColor InactiveButtonColor = FLinearColor(0.1f, 0.1f, 0.1f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "UI|Tab Styling")
	FLinearColor ActiveTextColor = FLinearColor::Black;
	UPROPERTY(EditAnywhere, Category = "UI|Tab Styling")
	FLinearColor InactiveTextColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	TArray<FInventoryTabInfo> TabInfos;
	TArray<FStatusTabInfo> StatusTabInfos;

	TMap<int32, class UTileView*> EquipmentSlotTiles;

	FOnItemHovered* OnItemHoveredDelegatePtr;
	FOnItemUnhovered* OnItemUnhoveredDelegatePtr;
	FEQUIP_ITEM* OnEquipItemDelegatePtr;
	FUNEQUIP_ITEM* OnUnEquipItemDelegatePtr;
	FOnStatusChanged* OnStatusChangedDelegatePtr;

protected:
	UFUNCTION()
	void OnEquipmentButtonClicked();

	UFUNCTION()
	void OnDisposableButtonClicked();

	UFUNCTION()
	void OnEquipedButtonClicked();

	UFUNCTION()
	void OnStatButtonClicked();

	UFUNCTION()
	void SetActiveTab(int32 CategoryToActivate);

	UFUNCTION()
	void SetActiveStatusTab(int32 CategoryToActivate);

	UFUNCTION()
	void OnStatusChanged(float AttackPower, float DefensePower, float HealthRegenPower);

public:
	virtual bool Initialize() override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	bool IsVisible();

	UFUNCTION(BlueprintPure, Category = "UI")
	bool IsDisabled() const { return Disabled; };

	UFUNCTION()
	void InsertItem(class UCItemData* ItemData);
	UFUNCTION()
	void RemoveItem(class UCItemData* ItemData);
	UFUNCTION()
	void EquipItem(class UCItemData* ItemData);
	UFUNCTION()
	void UnEquipItem(class UCItemData* ItemData);

	void UnEquipItem(int32 ItemType);

	FINSERT_ITEM* GetDelegate_InsertItem() { return &Delegate_InsertItem; };
	void Refresh_ItemTile();

	void SetDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate, FEQUIP_ITEM* EquipDelegate, FUNEQUIP_ITEM* UnEquipItemDelegate, FOnStatusChanged* StatusChangedDelegate);
};

