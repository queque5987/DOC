#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IPlayerState.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CPlayerState.generated.h"

UCLASS()
class DOC_API ACPlayerState : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()
	
	TArray<class UCItemData*> InventoryItems;
public:
	FINSERT_ITEM Delegate_INSERT_ITEM;
	FINSERT_ITEM* Delegate_UI_INSERT_ITEM;

	virtual void BeginPlay() override;

	virtual void GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem) override;
	virtual void SetUIInventoryDelegate(FINSERT_ITEM* Delegate_InsertItem) override;

	UFUNCTION()
	void InsertItem(class UCItemData* ItemData);
};
