#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
//#include "Interfaces/IPlayerControllerStage.h"
//#include "Interfaces/IPlayerOnStage.h"
#include "Player/UI/CItemData.h"

DECLARE_DELEGATE(FLIGHTS_ON);
/* Param : Character, Controller */
DECLARE_DELEGATE_TwoParams(FENTERED_STAGE, class UObject*, class UObject*);
DECLARE_DELEGATE_OneParam(FINSERT_ITEM, class UCItemData*);
DECLARE_DELEGATE_OneParam(FINSERT_EQUIPMENT, class UCItemData*);
DECLARE_MULTICAST_DELEGATE_OneParam(FEQUIP_ITEM, class UCItemData*);
DECLARE_MULTICAST_DELEGATE_OneParam(FUNEQUIP_ITEM, class UCItemData*);
DECLARE_DELEGATE_TwoParams(FHP_CHANGED, float, float);
DECLARE_DELEGATE_ThreeParams(FOnStatusChanged, float, float, float);
DECLARE_DELEGATE_OneParam(FMONTAGE_PLAYING_STATE_CHANGED, bool);
DECLARE_DELEGATE(FMONTAGE_PLAYER_COMBO_CLEARED);

DECLARE_DELEGATE_OneParam(FOnItemHovered, class UCItemData*);
DECLARE_DELEGATE(FOnItemUnhovered);

DECLARE_DELEGATE_TwoParams(FOnEquipmentChanged, int32, class USkeletalMesh*);

class DOC_API CStageDelegateTypes
{
public:

	CStageDelegateTypes();
	~CStageDelegateTypes();
};
