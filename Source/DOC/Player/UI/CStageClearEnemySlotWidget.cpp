#include "Player/UI/CStageClearEnemySlotWidget.h"
#include "Player/UI/CSpawnedEnemyData.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Materials/MaterialInstance.h"

class UCItemData;

UCStageClearEnemySlotWidget::UCStageClearEnemySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EnemyMaterialInstances.SetNum(ENEMYCHARACTER_NUM);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_MelleeFinder(TEXT("/Game/UI/RenderTarget/MI_Minion_Mellee.MI_Minion_Mellee")); // Placeholder path
	ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_RangedFinder(TEXT("/Game/UI/RenderTarget/MI_Minion_Ranged.MI_Minion_Ranged")); // Placeholder path
	ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_SiegeFinder(TEXT("/Game/UI/RenderTarget/MI_Minion_Siege.MI_Minion_Siege")); // Placeholder path

	if (MI_MelleeFinder.Succeeded()) EnemyMaterialInstances[ENEMYCHARACTER_MINION] = MI_MelleeFinder.Object;
	if (MI_RangedFinder.Succeeded()) EnemyMaterialInstances[ENEMYCHARACTER_MINION_RANGED] = MI_RangedFinder.Object;
	if (MI_SiegeFinder.Succeeded()) EnemyMaterialInstances[ENEMYCHARACTER_MINION_SIEZE] = MI_SiegeFinder.Object;
}

void UCStageClearEnemySlotWidget::AddNextDroppedItem()
{
	if (Enemy_DropItem)
	{
		if (DropItems.IsValidIndex(CurrentDroppedItemIdx) && DropItems[CurrentDroppedItemIdx] != nullptr)
		{
			Enemy_DropItem->AddItem(DropItems[CurrentDroppedItemIdx]);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(DroppedItemTimerHandle);
		}
	}
}

void UCStageClearEnemySlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UCSpawnedEnemyData* EnemyData = Cast<UCSpawnedEnemyData>(ListItemObject);
	if (EnemyData == nullptr) return;

	IIEnemyCharacter* EnemyCharacter = EnemyData->GetEnemyCharacter();
	if (EnemyCharacter == nullptr) return;

	if (Enemy_Thumbnail)
	{
		int32 EnemyType = EnemyCharacter->GetEnemyType();
		if (EnemyMaterialInstances.IsValidIndex(EnemyType) && EnemyMaterialInstances[EnemyType])
		{
			Enemy_Thumbnail->SetBrushFromMaterial(EnemyMaterialInstances[EnemyType]);
		}
	}

	if (Enemy_Name)
	{
		Enemy_Name->SetText(FText::FromString(EnemyCharacter->GetEnemyName()));
	}

	if (Enemy_EXP)
	{
		Enemy_EXP->SetText(FText::Format(NSLOCTEXT("StageClear", "EnemyExpFormat", "{0}"), FText::AsNumber(EnemyCharacter->GetExp())));
	}

	if (Enemy_DropItem)
	{
		Enemy_DropItem->ClearListItems();
		CurrentDroppedItemIdx = 0;
		DropItems = EnemyData->GetDroppedItems();
		GetWorld()->GetTimerManager().SetTimer(DroppedItemTimerHandle, this, &UCStageClearEnemySlotWidget::AddNextDroppedItem, 0.125f, true);
	}
}
