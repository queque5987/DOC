#include "CPotion.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IPlayerControllerUI.h"
#include "Interfaces/IGameModeDataManager.h"

ACPotion::ACPotion()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SMC_Potion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Potion"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> Potion1Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Potion-01.SM-Potion-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Potion2Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Potion-02.SM-Potion-02"));
	if (Potion1Finder.Succeeded()) SM_Potion_blue = Potion1Finder.Object;
	if (Potion2Finder.Succeeded()) SM_Potion_green = Potion2Finder.Object;

	SetRootComponent(SceneComponent);
	SMC_Potion->SetupAttachment(SceneComponent);
}

void ACPotion::BeginPlay()
{
	Super::BeginPlay();
}

void ACPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPotion::Select()
{
	if (Selected) return;
	Selected = true;
	if (SMC_Potion != nullptr)
	{
		SMC_Potion->SetRenderCustomDepth(true);
		SMC_Potion->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
}

void ACPotion::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (SMC_Potion != nullptr)
	{
		SMC_Potion->SetRenderCustomDepth(false);
	}
}

void ACPotion::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	if (bBusy) return;

	if (ItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ACPotion::Interact: ItemData is null."));
		return;
	}

	if (PlayerControllerUI != nullptr)
	{
		PlayerControllerUI->InsertItem(ItemData, this);
		UnSelect();
		bBusy = true; 
	}
}

void ACPotion::SetVisibility(bool e)
{
	if (e)
	{
		SceneComponent->Activate();
		SMC_Potion->Activate();
	}
	else
	{
		SceneComponent->Deactivate();
		SMC_Potion->Deactivate();
		SetActorLocation(FVector::ZeroVector);
	}
	SMC_Potion->SetVisibility(e);
	SMC_Potion->SetSimulatePhysics(e);
	SMC_Potion->SetCollisionEnabled(e ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	if (!e)
	{
		//SetActorTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::ZeroVector));
		SetActorScale3D(e ? FVector(1.f, 1.f, 1.f) : FVector(0.f, 0.f, 0.f));
		if (ChestArr != nullptr) (*ChestArr)[ChestIdx] = nullptr;
	}
}

bool ACPotion::GetVisibility()
{
	return SMC_Potion->IsActive();
}

void ACPotion::SetItemData(UCItemData* ItemDataAsset)
{
	ItemData = ItemDataAsset;
}

void ACPotion::SetItemType(int32 Type)
{
	//IIGameModeDataManager* GameModeDataManager = Cast<IIGameModeDataManager>(GetWorld()->GetAuthGameMode());
	switch (Type)
	{
	case(INTERACTABLE_ITEM_POTION_BLUE):
		if (SMC_Potion != nullptr && SM_Potion_blue != nullptr) SMC_Potion->SetStaticMesh(SM_Potion_blue);
		break;
	case(INTERACTABLE_ITEM_POTION_GREEN):
		if (SMC_Potion != nullptr && SM_Potion_green != nullptr) SMC_Potion->SetStaticMesh(SM_Potion_green);
		break;
	default:
		break;
	}
	ItemType = Type;
}

void ACPotion::SetChestSection(TArray<class IIInteractableItem*>* ChestItems, int32 idx)
{
	ChestArr = ChestItems;
	ChestIdx = idx;
}

