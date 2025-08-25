#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PCH.h"
#include "CStageClearEnemySlotWidget.generated.h"

UCLASS()
class DOC_API UCStageClearEnemySlotWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UCStageClearEnemySlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Enemy_Thumbnail;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Enemy_Name;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Enemy_EXP;

	UPROPERTY(meta = (BindWidget))
	class UTileView* Enemy_DropItem;

	TArray<class UCItemData*> DropItems;
	int8 CurrentDroppedItemIdx;
	FTimerHandle DroppedItemTimerHandle;
private:
	TArray<class UMaterialInstance*> EnemyMaterialInstances;

	void AddNextDroppedItem();
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
