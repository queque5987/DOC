#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IInteractableItem.h"
#include "PCH.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CGemstone.generated.h"

UCLASS()
class DOC_API ACGemstone : public AActor, public IIInteractableItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SMC_Gemstone;
	class UStaticMesh* SM_Gemstone_Blue;
	class UStaticMesh* SM_Gemstone_Yellow;
	class UStaticMesh* SM_Gemstone_Red;
	class UStaticMesh* SM_Gemstone_Green;
	class UStaticMesh* SM_Gemstone_Purple;
	class UStaticMesh* SM_Gemstone_Pink;
	class UCItemData* ItemData;
	int32 ItemType;
	TArray<IIInteractableItem*>* ChestArr;
	int32 ChestIdx;
public:	
	ACGemstone();

protected:
	virtual void BeginPlay() override;
	bool Selected = false;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual bool IsSelectable(class UPrimitiveComponent* HitComponent = nullptr) override { return true; };
	virtual void Interact(class IIPlayerControllerUI* PlayerControllerUI = nullptr, class IIPlayerControllerStage* PlayerControllerStage = nullptr) override;
	virtual void Select() override;
	virtual void UnSelect() override;
	virtual void SetVisibility(bool e) override;
	virtual bool GetVisibility() override;
	virtual void SetItemData(class UCItemData* ItemDataAsset) override;
	virtual void SetItemType(int32 Type) override;
	virtual int32 GetItemType() override { return ItemType; };
	virtual void SetChestSection(TArray<class IIInteractableItem*>* ChestItems, int32 idx) override;
};
