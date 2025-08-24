#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CStageClearEnemySlotWidget.generated.h"

UCLASS()
class DOC_API UCStageClearEnemySlotWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UCStageClearEnemySlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* Enemy_Thumbnail;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Enemy_Name;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Enemy_EXP;

	UPROPERTY(meta = (BindWidget))
	UTileView* Enemy_DropItem;

private:
	TArray<UMaterialInstance*> EnemyMaterialInstances;
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
