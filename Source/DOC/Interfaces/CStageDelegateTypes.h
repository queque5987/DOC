#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Player/UI/CItemData.h"

DECLARE_DELEGATE(FLIGHTS_ON);
/* Param : Character, Controller */
DECLARE_DELEGATE_TwoParams(FENTERED_STAGE, class UObject*, class UObject*);
DECLARE_DELEGATE_OneParam(FINSERT_ITEM, class UCItemData*);

class DOC_API CStageDelegateTypes
{
public:

	CStageDelegateTypes();
	~CStageDelegateTypes();
};
