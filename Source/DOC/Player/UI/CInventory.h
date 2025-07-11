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
	class UButton* Btn_Equipment;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Desposable;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Btn_Equipment;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Btn_Desposable;

	UPROPERTY(EditAnywhere, Category = "UI|Tab Styling")
	TArray<FLinearColor> ActiveButtonColorArr;
	
	UPROPERTY(EditAnywhere, Category = "UI|Tab Styling")
	FLinearColor InactiveButtonColor = FLinearColor(0.1f, 0.1f, 0.1f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "UI|Tab Styling")
	FLinearColor ActiveTextColor = FLinearColor::Black;
	UPROPERTY(EditAnywhere, Category = "UI|Tab Styling")
	FLinearColor InactiveTextColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	TArray<struct FInventoryTabInfo> TabInfos;

	FOnItemHovered* OnItemHoveredDelegatePtr;
	FOnItemUnhovered* OnItemUnhoveredDelegatePtr;

protected:
	UFUNCTION()
	void OnEquipmentButtonClicked();

	UFUNCTION()
	void OnDisposableButtonClicked();

	UFUNCTION()
	void SetActiveTab(int32 CategoryToActivate);

public:
	virtual bool Initialize() override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	bool IsVisible();

	UFUNCTION(BlueprintPure, Category = "UI")
	bool IsDisabled() const { return Disabled; };

	UFUNCTION()
	void InsertItem(class UCItemData* ItemData);

	FINSERT_ITEM* GetDelegate_InsertItem() { return &Delegate_InsertItem; };
	void Refresh_ItemTile();

	void SetItemTooltipDelegates(FOnItemHovered* HoveredDelegate, FOnItemUnhovered* UnhoveredDelegate);
};
