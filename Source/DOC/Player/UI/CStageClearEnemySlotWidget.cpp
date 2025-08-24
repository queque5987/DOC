#include "Player/UI/CStageClearEnemySlotWidget.h"
#include "Player/UI/CSpawnedEnemyData.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "PCH.h"
#include "Materials/MaterialInstance.h"

UCStageClearEnemySlotWidget::UCStageClearEnemySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EnemyMaterialInstances.SetNum(ENEMYCHARACTER_NUM);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_MelleeFinder(TEXT("/Game/Dungeon/Materials/M_Minion_0.M_Minion_0")); // Placeholder path
	ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_RangedFinder(TEXT("/Game/Dungeon/Materials/M_Minion_1.M_Minion_1")); // Placeholder path
	ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_SiegeFinder(TEXT("/Game/Dungeon/Materials/M_Minion_2.M_Minion_2")); // Placeholder path

	if (MI_MelleeFinder.Succeeded()) EnemyMaterialInstances[ENEMYCHARACTER_MINION] = MI_MelleeFinder.Object;
	if (MI_RangedFinder.Succeeded()) EnemyMaterialInstances[ENEMYCHARACTER_MINION_RANGED] = MI_RangedFinder.Object;
	if (MI_SiegeFinder.Succeeded()) EnemyMaterialInstances[ENEMYCHARACTER_MINION_SIEZE] = MI_SiegeFinder.Object;
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
		Enemy_EXP->SetText(FText::Format(NSLOCTEXT("StageClear", "EnemyExpFormat", "EXP: {0}"), FText::AsNumber(EnemyCharacter->GetExp())));
	}

	if (Enemy_DropItem)
	{
		Enemy_DropItem->ClearListItems();
		
		TArray<UObject*> DropItems = EnemyData->GetDropItems();
		for (UObject* DropItem : DropItems)
		{
			Enemy_DropItem->AddItem(DropItem);
		}
	}
}
