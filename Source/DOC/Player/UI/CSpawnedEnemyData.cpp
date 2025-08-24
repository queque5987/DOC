// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/CSpawnedEnemyData.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Player/UI/CItemData.h"

void UCSpawnedEnemyData::Initialize(IIEnemyCharacter* InEnemy, FOnDeath* InOnDiedCompletedDelegate, FDelegateHandle InOnDiedCompletedHandle)
{
	Enemy = InEnemy;
	OnDiedCompletedDelegate = InOnDiedCompletedDelegate;
	OnDiedCompletedHandle = InOnDiedCompletedHandle;
	bDead = false;
}

void UCSpawnedEnemyData::AddDroppedItem(class UCItemData* Item)
{
	DroppedItems.Add(Item);
}

TArray<class UCItemData*>& UCSpawnedEnemyData::GetDroppedItems()
{
	return DroppedItems;
}

const TArray<class UCItemData*>& UCSpawnedEnemyData::GetDroppedItems() const
{
	return DroppedItems;
}
