#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Interfaces/CStageStructs.h"
#include "Player/UI/CItemData.h"

DECLARE_DELEGATE(FLIGHTS_ON);
DECLARE_DELEGATE(FMONTAGE_PLAYER_COMBO_CLEARED);
DECLARE_DELEGATE(FOnItemUnhovered);
DECLARE_DELEGATE_OneParam(FINSERT_ITEM, class UCItemData*);
DECLARE_DELEGATE_OneParam(FINSERT_EQUIPMENT, class UCItemData*);
DECLARE_DELEGATE_OneParam(FMONTAGE_PLAYING_STATE_CHANGED, bool);
DECLARE_DELEGATE_OneParam(FOnItemHovered, class UCItemData*);
DECLARE_DELEGATE_TwoParams(FENTERED_STAGE, class UObject*, class UObject*);
DECLARE_DELEGATE_TwoParams(FHP_CHANGED, float, float);
DECLARE_DELEGATE_TwoParams(FOnEquipmentChanged, int32, class USkeletalMesh*);

DECLARE_MULTICAST_DELEGATE(FOnRefreshUI);
DECLARE_MULTICAST_DELEGATE(FOutOfMana);
DECLARE_MULTICAST_DELEGATE(FOnGroggyEnd);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGroggy, FDamageConfig);
DECLARE_MULTICAST_DELEGATE_OneParam(FGETITEM, class UCItemData*);
DECLARE_MULTICAST_DELEGATE_OneParam(FEQUIP_ITEM, class UCItemData*);
DECLARE_MULTICAST_DELEGATE_OneParam(FUNEQUIP_ITEM, class UCItemData*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatusChanged, FPlayerStat);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerInventoryChanged, const TArray<class UCItemData*>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeath, FDamageConfig);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnReceivedDamage, FDamageConfig);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuickslotChanged, const TArray<class UCItemData*>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeCounterReady, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuickSlotInput, int);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnExecute, FDamageConfig);
DECLARE_MULTICAST_DELEGATE_TwoParams(FStageCleared, class UObject*, const TArray<class UCItemData*>&)

class DOC_API CStageDelegateTypes
{
	CStageDelegateTypes();

	~CStageDelegateTypes();
};
