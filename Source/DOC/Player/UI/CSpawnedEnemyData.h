// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CSpawnedEnemyData.generated.h"

class IIEnemyCharacter;
class UCItemData;

/**
 * 
 */
UCLASS()
class DOC_API UCSpawnedEnemyData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Spawned Enemy Data")
	IIEnemyCharacter* Enemy;

	FOnDeath* OnDiedCompletedDelegate;

	UPROPERTY(VisibleAnywhere, Category = "Spawned Enemy Data")
	FDelegateHandle OnDiedCompletedHandle;

	UPROPERTY(VisibleAnywhere, Category = "Spawned Enemy Data")
	bool bDead;

	UPROPERTY(VisibleAnywhere, Category = "Spawned Enemy Data")
	TArray<UCItemData*> DroppedItems;

public:
	void Initialize(class IIEnemyCharacter* InEnemy, FOnDeath* InOnDiedCompletedDelegate, FDelegateHandle InOnDiedCompletedHandle);
	void AddDroppedItem(class UCItemData* Item);
	TArray<class UCItemData*>& GetDroppedItems();
	const TArray<class UCItemData*>& GetDroppedItems() const;
};