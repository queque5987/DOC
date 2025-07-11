#include "CGemstone.h"
#include "Player/UI/CItemData.h"
#include "Interfaces/IPlayerControllerUI.h"

ACGemstone::ACGemstone()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SMC_Gemstone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Gemstone"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> Gemstone1Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gemstone-01.SM-Gemstone-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Gemstone2Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gemstone-02.SM-Gemstone-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Gemstone3Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gemstone-03.SM-Gemstone-03"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Gemstone4Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gemstone-04.SM-Gemstone-04"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Gemstone5Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gemstone-05.SM-Gemstone-05"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Gemstone6Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gemstone-06.SM-Gemstone-06"));

	SetRootComponent(SceneComponent);
	SMC_Gemstone->SetupAttachment(SceneComponent);

	if (Gemstone1Finder.Succeeded()) SM_Gemstone_Blue = Gemstone1Finder.Object;
	if (Gemstone2Finder.Succeeded()) SM_Gemstone_Yellow = Gemstone2Finder.Object;
	if (Gemstone3Finder.Succeeded()) SM_Gemstone_Red = Gemstone3Finder.Object;
	if (Gemstone4Finder.Succeeded()) SM_Gemstone_Green = Gemstone4Finder.Object;
	if (Gemstone5Finder.Succeeded()) SM_Gemstone_Purple = Gemstone5Finder.Object;
	if (Gemstone6Finder.Succeeded()) SM_Gemstone_Pink = Gemstone6Finder.Object;

	if (SMC_Gemstone && SM_Gemstone_Blue != nullptr) SMC_Gemstone->SetStaticMesh(SM_Gemstone_Blue);
}

void ACGemstone::BeginPlay()
{
	Super::BeginPlay();

	SMC_Gemstone->SetSimulatePhysics(true);
}

void ACGemstone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACGemstone::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	if (bBusy) return;

	if (ItemData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ACGemstone::Interact: ItemData is null."));
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

void ACGemstone::Select()
{
	if (Selected) return;
	Selected = true;
	if (SMC_Gemstone != nullptr)
	{
		SMC_Gemstone->SetRenderCustomDepth(true);
		SMC_Gemstone->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
}

void ACGemstone::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (SMC_Gemstone != nullptr)
	{
		SMC_Gemstone->SetRenderCustomDepth(false);
	}
}

void ACGemstone::SetVisibility(bool e)
{
	if (e) SceneComponent->Activate();
	else SceneComponent->Deactivate();
	SMC_Gemstone->SetVisibility(e);
	SMC_Gemstone->SetSimulatePhysics(e);
	SMC_Gemstone->SetCollisionEnabled(e ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	if (!e)
	{
		SetActorTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::ZeroVector));
		if (ChestArr != nullptr) (*ChestArr)[ChestIdx] = nullptr;
	}
}

bool ACGemstone::GetVisibility()
{
	return SMC_Gemstone->IsActive();
}

void ACGemstone::SetItemData(UCItemData* ItemDataAsset)
{
	ItemData = ItemDataAsset;
}

void ACGemstone::SetItemType(int32 Type)
{
	switch (Type)
	{
	case(INTERACTABLE_ITEM_GEMSTONE_BLUE):
		if (SMC_Gemstone && SM_Gemstone_Blue != nullptr) SMC_Gemstone->SetStaticMesh(SM_Gemstone_Blue);
		break;
	case(INTERACTABLE_ITEM_GEMSTONE_YELLOW):
		if (SMC_Gemstone && SM_Gemstone_Yellow != nullptr) SMC_Gemstone->SetStaticMesh(SM_Gemstone_Yellow);
		break;
	case(INTERACTABLE_ITEM_GEMSTONE_RED):
		if (SMC_Gemstone && SM_Gemstone_Red != nullptr) SMC_Gemstone->SetStaticMesh(SM_Gemstone_Red);
		break;
	case(INTERACTABLE_ITEM_GEMSTONE_GREEN):
		if (SMC_Gemstone && SM_Gemstone_Green != nullptr) SMC_Gemstone->SetStaticMesh(SM_Gemstone_Green);
		break;
	case(INTERACTABLE_ITEM_GEMSTONE_PURPLE):
		if (SMC_Gemstone && SM_Gemstone_Purple != nullptr) SMC_Gemstone->SetStaticMesh(SM_Gemstone_Purple);
		break;
	case(INTERACTABLE_ITEM_GEMSTONE_PINK):
		if (SMC_Gemstone && SM_Gemstone_Pink != nullptr) SMC_Gemstone->SetStaticMesh(SM_Gemstone_Pink);
		break;
	default:
		break;
	}
	ItemType = Type;
}

void ACGemstone::SetChestSection(TArray<class IIInteractableItem*>* ChestItems, int32 idx)
{
	ChestArr = ChestItems;
	ChestIdx = idx;
}

