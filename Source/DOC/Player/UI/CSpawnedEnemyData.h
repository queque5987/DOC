#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CSpawnedEnemyData.generated.h"

class IIEnemyCharacter;
class UCItemData;

UCLASS()
class DOC_API UCSpawnedEnemyData : public UDataAsset
{
	GENERATED_BODY()

public:
	IIEnemyCharacter* Enemy;
	FOnDeath* OnDiedCompletedDelegate;
	FDelegateHandle OnDiedCompletedHandle;
	UPROPERTY(VisibleAnywhere, Category = "Spawned Enemy Data")
	bool bDead;
	UPROPERTY(VisibleAnywhere, Category = "Spawned Enemy Data")
	TArray<UCItemData*> DroppedItems;

public:
	void Initialize(class IIEnemyCharacter* InEnemy, FOnDeath* InOnDiedCompletedDelegate, FDelegateHandle InOnDiedCompletedHandle);
	void AddDroppedItem(class UCItemData* Item);
	IIEnemyCharacter* GetEnemyCharacter() { return Enemy; }
	TArray<class UCItemData*>& GetDroppedItems();
	const TArray<class UCItemData*>& GetDroppedItems() const;
};