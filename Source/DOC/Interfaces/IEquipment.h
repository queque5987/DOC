#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IEquipment.generated.h"

UINTERFACE(MinimalAPI)
class UIEquipment : public UInterface
{
	GENERATED_BODY()
};

class DOC_API IIEquipment
{
	GENERATED_BODY()

public:
	virtual void SetEqipmentType(int32 Type) {};
	virtual int32 GetEquipmentType() { return -1; };
	virtual void Equip(bool e) {};
	virtual class USkeletalMesh* GetSkeletalMesh() { return nullptr; };
};
