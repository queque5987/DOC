#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CInventory.generated.h"

UCLASS()
class DOC_API UCInventory : public UUserWidget
{
	GENERATED_BODY()

	private:
	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	bool Disabled = false;
	float DisableSec = 0.85f;
	FTimerHandle DisableTimer;
	FINSERT_ITEM Delegate_InsertItem;

	UPROPERTY(meta = (BindWidget))
	class UTileView* ItemTile;

	FOnItemHovered* OnItemHoveredDelegatePtr;
	FOnItemUnhovered* OnItemUnhoveredDelegatePtr;

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
