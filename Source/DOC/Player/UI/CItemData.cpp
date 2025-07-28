#include "CItemData.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

UCItemData::UCItemData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> DragVisualBPClass(TEXT("/Game/UI/BP_InventoryItem_Drag")); // Replace with your actual blueprint path
	if (DragVisualBPClass.Class != nullptr)
	{
		DragVisualClass = DragVisualBPClass.Class;
	}
}

