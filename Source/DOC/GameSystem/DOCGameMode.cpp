#include "DOCGameMode.h"
#include "Player/DOCCharacter.h"
#include "Player/CPlayerState.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DataAsset.h"
#include "Player/UI/CItemData.h"

ADOCGameMode::ADOCGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_PlayableCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PlayerStateClass = ACPlayerState::StaticClass();

	ConstructorHelpers::FObjectFinder<UCItemData> Potion1Finder(TEXT("/Game/Data/Item/DataAsset/DA_Potion1.DA_Potion1"));
	ConstructorHelpers::FObjectFinder<UCItemData> Potion2Finder(TEXT("/Game/Data/Item/DataAsset/DA_Potion2.DA_Potion2"));
	ConstructorHelpers::FObjectFinder<UCItemData> Gemstone1Finder(TEXT("/Game/Data/Item/DataAsset/DA_Gemstone1.DA_Gemstone1"));
	ConstructorHelpers::FObjectFinder<UCItemData> Gemstone2Finder(TEXT("/Game/Data/Item/DataAsset/DA_Gemstone2.DA_Gemstone2"));
	ConstructorHelpers::FObjectFinder<UCItemData> Gemstone3Finder(TEXT("/Game/Data/Item/DataAsset/DA_Gemstone3.DA_Gemstone3"));
	ConstructorHelpers::FObjectFinder<UCItemData> Gemstone4Finder(TEXT("/Game/Data/Item/DataAsset/DA_Gemstone4.DA_Gemstone4"));
	ConstructorHelpers::FObjectFinder<UCItemData> Gemstone5Finder(TEXT("/Game/Data/Item/DataAsset/DA_Gemstone5.DA_Gemstone5"));
	ConstructorHelpers::FObjectFinder<UCItemData> Gemstone6Finder(TEXT("/Game/Data/Item/DataAsset/DA_Gemstone6.DA_Gemstone6"));
	
	ConstructorHelpers::FObjectFinder<UCItemData> SwordFinder(TEXT("/Game/Data/Item/DataAsset/DA_Sword.DA_Sword"));
	ConstructorHelpers::FObjectFinder<UCItemData> GloveFinder(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Glove.DA_Glove"));
	ConstructorHelpers::FObjectFinder<UCItemData> MaskFinder(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Mask.DA_Mask"));
	ConstructorHelpers::FObjectFinder<UCItemData> PantsFinder(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Pants.DA_Pants"));
	ConstructorHelpers::FObjectFinder<UCItemData> ShoseFinder(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Shose.DA_Shose"));
	ConstructorHelpers::FObjectFinder<UCItemData> Torso1Finder(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Torso1.DA_Torso1"));
	ConstructorHelpers::FObjectFinder<UCItemData> Torso2Finder(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Torso2.DA_Torso2"));
	ConstructorHelpers::FObjectFinder<UCItemData> Torso3Finder(TEXT("/Game/Data/Item/DataAsset/Equipment/DA_Torso3.DA_Torso3"));

	ItemAssets.SetNum(INTERACTABLE_ITEM_NUM);
	if (Potion1Finder.Succeeded())		ItemAssets[INTERACTABLE_ITEM_POTION_BLUE]		= Potion1Finder.Object;
	if (Potion2Finder.Succeeded())		ItemAssets[INTERACTABLE_ITEM_POTION_GREEN]		= Potion2Finder.Object;
	if (Gemstone1Finder.Succeeded())	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_BLUE]		= Gemstone1Finder.Object;
	if (Gemstone2Finder.Succeeded())	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_YELLOW]	= Gemstone2Finder.Object;
	if (Gemstone3Finder.Succeeded())	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_RED]		= Gemstone3Finder.Object;
	if (Gemstone4Finder.Succeeded())	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_GREEN]	= Gemstone4Finder.Object;
	if (Gemstone5Finder.Succeeded())	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_PURPLE]	= Gemstone5Finder.Object;
	if (Gemstone6Finder.Succeeded())	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_PINK]		= Gemstone6Finder.Object;

	EquipmentAssets.SetNum(EQUIPMENT_NUM);
	if (SwordFinder.Succeeded())	EquipmentAssets[EQUIPMENT_SWORD] = SwordFinder.Object;
	if (GloveFinder.Succeeded())	EquipmentAssets[EQUIPMENT_GLOVE] = GloveFinder.Object;
	if (MaskFinder.Succeeded())		EquipmentAssets[EQUIPMENT_HELMET] = MaskFinder.Object;
	if (PantsFinder.Succeeded())	EquipmentAssets[EQUIPMENT_PANTS] = PantsFinder.Object;
	if (ShoseFinder.Succeeded())	EquipmentAssets[EQUIPMENT_SHOSE] = ShoseFinder.Object;
	if (Torso1Finder.Succeeded())	EquipmentAssets[EQUIPMENT_TORSO1] = Torso1Finder.Object;
	if (Torso2Finder.Succeeded())	EquipmentAssets[EQUIPMENT_TORSO2] = Torso2Finder.Object;
	if (Torso3Finder.Succeeded())	EquipmentAssets[EQUIPMENT_TORSO3] = Torso3Finder.Object;

	ItemAssets[INTERACTABLE_ITEM_POTION_BLUE]->ItemCode = INTERACTABLE_ITEM_POTION_BLUE;
	ItemAssets[INTERACTABLE_ITEM_POTION_GREEN]->ItemCode = INTERACTABLE_ITEM_POTION_GREEN;
	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_BLUE]->ItemCode = INTERACTABLE_ITEM_GEMSTONE_BLUE;
	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_YELLOW]->ItemCode = INTERACTABLE_ITEM_GEMSTONE_YELLOW;
	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_RED]->ItemCode = INTERACTABLE_ITEM_GEMSTONE_RED;
	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_GREEN]->ItemCode = INTERACTABLE_ITEM_GEMSTONE_GREEN;
	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_PURPLE]->ItemCode = INTERACTABLE_ITEM_GEMSTONE_PURPLE;
	ItemAssets[INTERACTABLE_ITEM_GEMSTONE_PINK]->ItemCode = INTERACTABLE_ITEM_GEMSTONE_PINK;
}

UCItemData* ADOCGameMode::GetItemDataAsset(int32 ItemType)
{
	return ItemAssets.IsValidIndex(ItemType) ? ItemAssets[ItemType] : nullptr;
}

UCItemData* ADOCGameMode::GetEquipmentDataAsset(int32 EquipmentType)
{
	return EquipmentAssets.IsValidIndex(EquipmentType) ? EquipmentAssets[EquipmentType] : nullptr;
}
