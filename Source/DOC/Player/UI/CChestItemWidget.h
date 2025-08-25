#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CChestItemWidget.generated.h"

UCLASS()
class DOC_API UCChestItemWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UTileView* ItemList;

	UPROPERTY(meta = (BindWidget))
	class UButton* GetItemButton;

	UPROPERTY(meta = (BindWidget))
	class UBorder* BackgroundBorder;

	UPROPERTY(meta = (BindWidget))
	class UBorder* AroundBorder;
	
	FOnItemHovered* ItemHoveredDelegatePtr;
	FOnItemUnhovered* ItemUnHoveredDelegatePtr;
	FGETITEM* GetItemDelegatePtr;
	FPressedKeyboard* PressedKeyboardDelegatePtr;

	FTimerHandle DisplayTimerHandle;
	int32 DisplayItemIndex;
	int32 ChestItemTopRarity = 1;
	TArray<class UCItemData*>* DisplayItemDataArr;

	FLinearColor DefaultBorderColor;
	FLinearColor SourceColor;
	FLinearColor TargetColor;

	float TransitionProgress;
	float FadeInDuration = 0.2f;
	float FadeOutDuration = 0.5f;

public:
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OpenChest(TArray<class UCItemData*>* ToShowItemData);
	void SetDelegates(FOnItemHovered* InItemHoveredDelegatePtr, FOnItemUnhovered* InItemUnHoveredDelegatePtr, FGETITEM* InGetItemDelegatePtr);
	void SetKeyboardDelegate(FPressedKeyboard* InPressedKeyboardDelegatePtr);
private:
	void ChestItemShowIter();

	UFUNCTION()
	void OnGetItemButtonClicked();

	UFUNCTION()
	void OnGetItem(class UCItemData* InItemData);

	UFUNCTION()
	void OnPressedKeyboard(FKey Key);
};

