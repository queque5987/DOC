#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CInventory.generated.h"

UCLASS()
class DOC_API UCInventory : public UUserWidget
{
	GENERATED_BODY()

	bool Disabled = false;
	float DisableSec = 0.15f;
	FTimerHandle DisableTimer;
	FINSERT_ITEM Delegate_InsertItem;

	UPROPERTY(meta = (BindWidget))
	class UTileView* ItemTile;

public:
	virtual bool Initialize() override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	bool IsVisible();
	UFUNCTION()
	void InsertItem(class UCItemData* ItemData);
	FINSERT_ITEM* GetDelegate_InsertItem() { return &Delegate_InsertItem; };
	void Refresh_ItemTile();
};
