#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IEquipment.h"
#include "PCH.h"
#include "CSword.generated.h"

UCLASS()
class DOC_API ACSword : public AActor, public IIInteractableItem, public IIEquipment
{
	GENERATED_BODY()
	
	//UPROPERTY(EditAnywhere, Category = "Mesh")
	//class USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class USkeletalMeshComponent* SMC_Sword;

	class USkeletalMesh* SKMesh_Sword;

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
	ACSword();

protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

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
};
