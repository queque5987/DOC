#include "Player/Equipment/CSword.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IPlayerControllerUI.h"

ACSword::ACSword()
{
	PrimaryActorTick.bCanEverTick = true;
	//SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SMC_Sword = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshFinder(TEXT("/Game/QuangPhan/G2_Mercenaries/Meshes/Weapons/SK_Sword.SK_Sword"));
	if (SKMeshFinder.Succeeded()) SKMesh_Sword = SKMeshFinder.Object;
	SetRootComponent(SMC_Sword);
	//SMC_Sword->SetupAttachment(RootComponent);
	SMC_Sword->SetGenerateOverlapEvents(true);
	SMC_Sword->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SMC_Sword->SetCollisionResponseToChannel(COLLISION_CHANNEL_PLAYER_GAZE, ECollisionResponse::ECR_Block);
	SMC_Sword->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	SMC_Sword->SetSimulatePhysics(true);
	SMC_Sword->SetSkeletalMesh(SKMesh_Sword);
}

void ACSword::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACSword::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	if (bBusy || bEquipped) return;

	if (ItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ACSword::Interact: ItemData is null."));
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

void ACSword::Select()
{
	if (Selected || bEquipped) return;
	Selected = true;
	if (SMC_Sword != nullptr)
	{
		SMC_Sword->SetRenderCustomDepth(true);
		SMC_Sword->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
}

void ACSword::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (SMC_Sword != nullptr)
	{
		SMC_Sword->SetRenderCustomDepth(false);
	}
}

void ACSword::SetVisibility(bool e)
{
	if (SMC_Sword != nullptr)
	{
		if (e)
		{
			//SceneComponent->Activate();
			SMC_Sword->Activate();
		}
		else
		{
			//SceneComponent->Deactivate();
			SMC_Sword->Deactivate();
			SetActorTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::ZeroVector));
			if (ChestArr != nullptr) (*ChestArr)[ChestIdx] = nullptr;
		}
		SMC_Sword->SetVisibility(e);
		SMC_Sword->SetHiddenInGame(!e);
	}
	SetActorEnableCollision(e);
	UnSelect();
}

void ACSword::SetChestSection(TArray<class IIInteractableItem*>* ChestItems, int32 idx)
{
	ChestArr = ChestItems;
	ChestIdx = idx;
}

void ACSword::SetItemData(UCItemData* ItemDataAsset)
{
	ItemData = ItemDataAsset;
}

void ACSword::SetItemType(int32 Type)
{
	ItemType = Type;
}

void ACSword::SetEqipmentType(int32 Type)
{
	EquipmentType = Type;
	switch (Type)
	{
	case(EQUIPMENT_SWORD):
		SMC_Sword->SetSkeletalMesh(SKMesh_Sword);
		break;
	default:
		break;
	}
}

void ACSword::Equip(bool e)
{
	UnSelect();
	bEquipped = e;
	SetActorScale3D(FVector(1.f, 1.f, 1.f));
	//FTransform SocketTransform = SMC_Sword->GetSocketTransform(TEXT("Sword_root"), RTS_Actor);
	//SMC_Sword->SetRelativeLocation(-SocketTransform.GetLocation());
	SMC_Sword->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SMC_Sword->SetCollisionResponseToChannel(COLLISION_CHANNEL_PLAYER_GAZE, e ? ECollisionResponse::ECR_Ignore : ECollisionResponse::ECR_Block);
	SMC_Sword->SetCollisionResponseToChannel(ECC_Visibility, e ? ECollisionResponse::ECR_Ignore : ECollisionResponse::ECR_Block);
}
