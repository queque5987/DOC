#include "Player/Equipment/CWearables.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/IPlayerControllerUI.h"
#include "Player/UI/CItemData.h"

ACWearables::ACWearables()
{
	PrimaryActorTick.bCanEverTick = true;

	SM_Wearable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(SM_Wearable);

	SKMesh_Wearable = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	//SKMesh_Wearable->SetupAttachment(RootComponent);

	// Initially, the skeletal mesh is not visible
	SKMesh_Wearable->SetVisibility(false);
	SKMesh_Wearable->SetHiddenInGame(true);

	SM_Wearable->SetGenerateOverlapEvents(true);
	SM_Wearable->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SM_Wearable->SetCollisionResponseToChannel(COLLISION_CHANNEL_PLAYER_GAZE, ECollisionResponse::ECR_Block);
	SM_Wearable->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	SM_Wearable->SetSimulatePhysics(true);
}

void ACWearables::BeginPlay()
{
	Super::BeginPlay();
}

void ACWearables::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	if (bBusy || bEquipped) return;

	if (ItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ACWearables::Interact: ItemData is null."));
		return;
	}

	if (PlayerControllerUI != nullptr)
	{
		PlayerControllerUI->InsertItem(ItemData, this);

		UnSelect();
		SetVisibility(false);

		bBusy = true;
	}
}

void ACWearables::Select()
{
	if (Selected || bEquipped) return;
	Selected = true;
	if (SM_Wearable != nullptr)
	{
		SM_Wearable->SetRenderCustomDepth(true);
		SM_Wearable->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
}

void ACWearables::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (SM_Wearable != nullptr)
	{
		SM_Wearable->SetRenderCustomDepth(false);
	}
}

void ACWearables::SetVisibility(bool e)
{
	if (SM_Wearable != nullptr)
	{
		if (e)
		{
			SM_Wearable->Activate();
		}
		else
		{
			SM_Wearable->Deactivate();
			SetActorTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::ZeroVector));
d			if (ChestArr != nullptr) (*ChestArr)[ChestIdx] = nullptr;
		}
		SM_Wearable->SetVisibility(e);
		SM_Wearable->SetHiddenInGame(!e);
	}
	SetActorEnableCollision(e);
	UnSelect();
}

void ACWearables::SetChestSection(TArray<class IIInteractableItem*>* ChestItems, int32 idx)
{
	ChestArr = ChestItems;
	ChestIdx = idx;
}

void ACWearables::SetItemData(UCItemData* ItemDataAsset)
{
	ItemData = ItemDataAsset;
}

void ACWearables::SetItemType(int32 Type)
{
	ItemType = Type;
}

void ACWearables::SetEqipmentType(int32 Type)
{
	EquipmentType = Type;
	if (SM_Wearable != nullptr) SM_Wearable->SetStaticMesh(StaticMesh_Dropped);
	if (SKMesh_Wearable != nullptr) SKMesh_Wearable->SetSkeletalMesh(SkeletalMesh_Wearing);
}

void ACWearables::Equip(bool e)
{
	UnSelect();
	bEquipped = e;

	if (SM_Wearable != nullptr)
	{
		SM_Wearable->SetVisibility(!e);
		SM_Wearable->SetHiddenInGame(e);
		SM_Wearable->SetCollisionEnabled(e ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
		SM_Wearable->Activate(!e);
	}
	if (SKMesh_Wearable != nullptr)
	{
		SKMesh_Wearable->SetVisibility(e);
		SKMesh_Wearable->SetHiddenInGame(!e);
		SKMesh_Wearable->Activate(e);
	}

	if (e)
	{
		// TODO: Attach the SKMesh_Wearable to the character's mesh.
		// For example: SKMesh_Wearable->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "SocketName");
	}
	else
	{
		// Detach from character if needed
	}
}

USkeletalMesh* ACWearables::GetSkeletalMesh()
{
	return SkeletalMesh_Wearing;
}

int32 ACWearables::GetEquipmentType()
{
	return EquipmentType;
}

