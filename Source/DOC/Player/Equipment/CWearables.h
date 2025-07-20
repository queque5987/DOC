#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IEquipment.h"
#include "PCH.h"
#include "CWearables.generated.h"

UCLASS()
class DOC_API ACWearables : public AActor, public IIInteractableItem, public IIEquipment
{
	GENERATED_BODY()

private:


	const int32 ItemCategory = ITEM_CATEGORY_EQUIPMENT;
	int32 EquipmentType;
	class UCItemData* ItemData;
	int32 ItemType;
	TArray<IIInteractableItem*>* ChestArr;
	int32 ChestIdx;
	bool Selected = false;
	bool bEquipped = false;
	bool bBusy = false;

public:
	//UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* SM_Wearable;
	//UPROPERTY(EditAnywhere, Category = "Components")
	class USkeletalMeshComponent* SKMesh_Wearable;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class UStaticMesh* StaticMesh_Dropped;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class USkeletalMesh* SkeletalMesh_Wearing;

	ACWearables();
	virtual void BeginPlay() override;

	/*
		Interactable
	*/
	virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) override;
	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	virtual void Select() override;
	virtual void UnSelect() override;
	virtual void SetVisibility(bool e) override;
	virtual void SetChestSection(TArray<class IIInteractableItem*>* ChestItems, int32 idx) override;
	virtual const int32 GetItemCategory() override { return ItemCategory; };
	virtual void SetItemData(class UCItemData* ItemDataAsset) override;
	virtual void SetItemType(int32 Type) override;
	virtual int32 GetItemType() override { return ItemType; };

	/*
		Equipment
	*/
	virtual void SetEqipmentType(int32 Type) override;
	virtual void Equip(bool e) override;
	virtual int32 GetEquipmentType() override;
	virtual class USkeletalMesh* GetSkeletalMesh() override;
};
