#include "CChest.h"
#include "Interfaces/IObjectPoolManager.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/IGeneratedStage.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerControllerUI.h"
#include "Interfaces/IEquipment.h"
#include "Components/RectLightComponent.h"

ACChest::ACChest()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> Chest_TopFinder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Chest-04.SM-Chest-04"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Chest_BodyFinder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Chest-03.SM-Chest-03"));

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SM_Chest_Top = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Chest_Top"));
	SM_Chest_Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Chest_Body"));
	RectLight = CreateDefaultSubobject<URectLightComponent>(TEXT("RectLight"));

	if (Chest_TopFinder.Succeeded() && SM_Chest_Top != nullptr) SM_Chest_Top->SetStaticMesh(Chest_TopFinder.Object);
	if (Chest_BodyFinder.Succeeded() && SM_Chest_Body != nullptr) SM_Chest_Body->SetStaticMesh(Chest_BodyFinder.Object);

	SetRootComponent(SceneComponent);
	if (SM_Chest_Body != nullptr) SM_Chest_Body->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale); 
 	if (SM_Chest_Top != nullptr)
	{
		SM_Chest_Top->AttachToComponent(SM_Chest_Body, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SM_Chest_Top->SetRelativeLocation(FVector(0.f, -40.f, 50.f));
		SM_Chest_Top->SetRelativeRotation(FRotator(0.f, 0.f, 67.f));
	}

	RectLight->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	RectLight->SetRelativeLocation(FVector(0, 0, 100));
	RectLight->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	RectLight->SetIntensity(0.f);
	Angle = 67.f;
	SpawnedItems.SetNum(8);


	SpawnItemScales.Add(EQUIPMENT_SWORD,	FVector(1.2f, 0.5f, 1.2f));
	SpawnItemScales.Add(EQUIPMENT_HELMET,	FVector(1.f, 1.f, 1.f));
	SpawnItemScales.Add(EQUIPMENT_GLOVE,	FVector(1.f, 1.f, 1.f));
	SpawnItemScales.Add(EQUIPMENT_SHOSE,	FVector(1.f, 1.f, 1.f));
	SpawnItemScales.Add(EQUIPMENT_TORSO1,	FVector(0.5f, 0.5f, 0.5f));
	SpawnItemScales.Add(EQUIPMENT_TORSO2,	FVector(0.5f, 0.5f, 0.5f));
	SpawnItemScales.Add(EQUIPMENT_TORSO3,	FVector(0.5f, 0.5f, 0.5f));
	SpawnItemScales.Add(EQUIPMENT_PANTS,	FVector(0.5f, 0.5f, 0.5f));
}

void ACChest::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void ACChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MaxAngle < ToggleAngle) // Open
	{
		if (Angle > ToggleAngle * 0.85f) Angle -= DeltaTime * (ToggleAngle / OpenSpeed) * 3.f;
		else if (Angle > ToggleAngle * 0.75f) Angle -= DeltaTime * (ToggleAngle / OpenSpeed) * 0.3f;
		else Angle -= DeltaTime * (ToggleAngle / OpenSpeed) * 1.5f;

		if (Angle <= MaxAngle) // 0.f
		{
			Angle = MaxAngle;
			SetActorTickEnabled(false);
			bBusy = false;
			if (SM_Chest_Body != nullptr) SM_Chest_Body->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
			if (SM_Chest_Top != nullptr) SM_Chest_Top->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
		}
	}
	else // Close
	{
		Angle += DeltaTime * (MaxAngle / OpenSpeed) * 2.f;
		if (Angle >= MaxAngle) // 67.f
		{
			Angle = MaxAngle;
			SetActorTickEnabled(false);
			bBusy = false;
			if (SM_Chest_Body != nullptr) SM_Chest_Body->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
			if (SM_Chest_Top != nullptr) SM_Chest_Top->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
		}
	}
	SM_Chest_Top->SetRelativeRotation(FRotator(0.f, 0.f, Angle));
}

void ACChest::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	ToggleBox();
	if (!PlayerControllerUI->GetHasWeapon() && PlayerControllerStage != nullptr)
	{
		IIObjectPoolManager* ObjectPoolManager = PlayerControllerStage->GetObjectPoolManager();
		if (ObjectPoolManager != nullptr)
		{
			PlayerControllerUI->SetHasWeapon(true);
			PossessItems.Add(
				ObjectPoolManager->GetItemData(
					1,
					0,
					1
				)
			);
		}
	}
	FVector CamLoc = GetActorLocation() + (GetActorRightVector() + GetActorUpVector() * 3.f + GetActorForwardVector() * 2.f).GetSafeNormal() * 155.f;
	FRotator CamRot = (GetActorLocation() - CamLoc).GetSafeNormal().Rotation();
	if (MaxAngle == 67.f)
	{
		PlayerControllerStage->SetToFollowCamera();
		PlayerControllerUI->CloseChestItemWidget();
	}
	else
	{
		PlayerControllerStage->SetToPerspectiveCamera(FTransform(CamRot, CamLoc, FVector(1.f, 1.f, 1.f)));
		PlayerControllerUI->OpenChestItemWidget(&PossessItems);
	}

}

void ACChest::ClearItemData()
{
	PossessItems.Empty();
}

void ACChest::AddItemData(UCItemData* InItemData)
{
	if (InItemData != nullptr) PossessItems.Add(InItemData);
}

bool ACChest::IsSelectable(UPrimitiveComponent* HitComponent)
{
	if (Angle < 67.f && HitComponent != SM_Chest_Top) return false;
	return true;
}

void ACChest::Select()
{
	if (Selected) return;
	Selected = true;
	if (SM_Chest_Top != nullptr)
	{
		SM_Chest_Top->SetRenderCustomDepth(true);
		SM_Chest_Top->SetCustomDepthStencilValue(bBusy ? CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY : CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
	if (SM_Chest_Body != nullptr)
	{
		SM_Chest_Body->SetRenderCustomDepth(true);
		SM_Chest_Body->SetCustomDepthStencilValue(bBusy ? CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY : CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
}

void ACChest::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (SM_Chest_Top != nullptr)
	{
		SM_Chest_Top->SetRenderCustomDepth(false);
	}
	if (SM_Chest_Body != nullptr)
	{
		SM_Chest_Body->SetRenderCustomDepth(false);
	}
}

void ACChest::SetVisibility(bool e)
{
	if (e)
	{
		SceneComponent->Activate();
		SM_Chest_Top->Activate();
		SM_Chest_Body->Activate();
	}
	else
	{
		if (Angle != 67.f)
		{
			ToggleAngle = 0.f;
			MaxAngle = 67.f;
			Angle = 67.f;
			SM_Chest_Top->SetRelativeRotation(FRotator(0.f, 0.f, Angle));
		}
		SceneComponent->Deactivate();
		SM_Chest_Top->Deactivate();
		SM_Chest_Body->Deactivate();
	}
	SM_Chest_Top->SetVisibility(e);
	SM_Chest_Body->SetVisibility(e);
	SM_Chest_Top->SetHiddenInGame(!e);
	SM_Chest_Body->SetHiddenInGame(!e);
	SetActorEnableCollision(e);
	UnSelect();
}

bool ACChest::GetVisibility()
{
	return (SM_Chest_Top->IsActive() && SM_Chest_Body->IsActive() ? true : false);
}

IIInteractableItem* ACChest::SpawnItemToStage(int32 ItemType, IIObjectPoolManager* ObjectPoolManager)
{
	if (ObjectPoolManager == nullptr) return nullptr;

	int32 idx = FMath::Floor(FMath::FRandRange(0.f, 8.f));
	int32 i = 0;
	bool bEmpty = false;
	for (; i < 8; i++)
	{
		if (SpawnedItems[(idx + i) % 8] == nullptr)
		{
			bEmpty = true;
			break;
		}
	}
	if (!bEmpty) return nullptr;
	idx += i;
	idx %= 8;

	float X = idx < 4 ? -15.f : 15.f;
	float Y = -45.f + idx % 4 * 30.f;
	float Z = 25.f;
	if ((int32)FMath::Abs(GetActorRotation().Yaw) % 180 == 0) Swap(X, Y);
	SpawnedItems[idx] = ObjectPoolManager->GetItem_InChest(this, ItemType, FTransform(
		FRotator(
			(FMath::FRandRange(0.f, 0.4f) - 0.2f) * 360.f,
			(FMath::FRandRange(0.f, 0.4f) - 0.2f) * 360.f,
			(FMath::FRandRange(0.f, 0.4f) - 0.2f) * 360.f
		),
		FVector(X, Y, Z), FVector(0.4f, 0.4f, 0.4f)
		)
	);
	SpawnedItems[idx]->SetChestSection(&SpawnedItems, idx);
	return SpawnedItems[idx];
}

IIInteractableItem* ACChest::SpawnEquipmentToStage(int32 EquipmentType, IIObjectPoolManager* ObjectPoolManager)
{
	if (ObjectPoolManager == nullptr) return nullptr;

	int32 idx = FMath::Floor(FMath::FRandRange(0.f, 8.f));
	int32 i = 0;
	bool bEmpty = false;
	for (; i < 8; i++)
	{
		if (SpawnedItems[(idx + i) % 8] == nullptr)
		{
			bEmpty = true;
			break;
		}
	}
	idx += i;
	idx %= 8;
	if (!bEmpty)
	{
		if (SpawnedItems[idx]->GetItemCategory() == ITEM_CATEGORY_DISPOSABLE)
		{
			ObjectPoolManager->ReturnItem(Cast<AActor>(SpawnedItems[idx]), SpawnedItems[idx]->GetItemType());
			SpawnedItems[idx] = nullptr;
		}
		else return nullptr;
	}

	float X = idx < 4 ? -1.5f : 1.5f;
	float Y = -4.5f + idx % 4 * 3.f;
	float Z = 15.f;
	if ((int32)FMath::Abs(GetActorRotation().Yaw) % 180 == 0) Swap(X, Y);
	SpawnedItems[idx] = Cast<IIInteractableItem>(
		ObjectPoolManager->GetEquipment(this, EquipmentType,
			FTransform(
				GetActorRotation(),
				GetActorLocation() + FVector(X, Y, Z),
				SpawnItemScales.Find(EquipmentType) ? SpawnItemScales[EquipmentType] : FVector(0.5f, 0.5f, 0.5f)
			)
		)
	);
	if (SpawnedItems[idx] != nullptr) SpawnedItems[idx]->SetChestSection(&SpawnedItems, idx);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACChest::SpawnEquipmentToStage: Failed to spawn equipment for type% d"), EquipmentType);
	}
	return SpawnedItems[idx];
}

void ACChest::ReturnItemsFromStage(IIObjectPoolManager* ObjectPoolManager, TArray<TArray<int32>>& SpawnItems)
{
	for(TArray<int32>& SpawnItem : SpawnItems)
	{
		SpawnItem.Empty();
	}
	for (int32 i = 0; i < 8; i++)
	{
		if (SpawnedItems[i] == nullptr) continue;
		SpawnItems[SpawnedItems[i]->GetItemCategory()].Add(SpawnedItems[i]->GetItemType());
		if (SpawnedItems[i]->GetItemCategory() == ITEM_CATEGORY_DISPOSABLE)
		{
			ObjectPoolManager->ReturnItem(SpawnedItems[i], SpawnedItems[i]->GetItemType());
		}
		else if (SpawnedItems[i]->GetItemCategory() == ITEM_CATEGORY_EQUIPMENT)
		{
			if (IIEquipment* Equipment = Cast<IIEquipment>(SpawnedItems[i]))
			{
				ObjectPoolManager->ReturnEquipment(Equipment, SpawnedItems[i]->GetItemType());
			}
		}
		SpawnedItems[i] = nullptr;
	}
}

void ACChest::ManualInteract(int32 OpenState, bool bForce)
{
	bool bOpen = (OpenState == INTERACTABLE_ITEM_STATE_CLOSED ? false : true);
	if (Angle != (bOpen ? 0.f : 67.f))
	{
		ToggleAngle = bOpen ? 67.f : 0.f;
		MaxAngle = bOpen ? 0.f : 67.f;
		Angle = bOpen ? 0.f : 67.f;
		SM_Chest_Top->SetRelativeRotation(FRotator(0.f, 0.f, Angle));
	}
}

void ACChest::ToggleBox()
{
	if (bBusy) return;

	UnSelect();
	bBusy = true;
	if (SM_Chest_Body != nullptr) SM_Chest_Body->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY);
	if (SM_Chest_Top != nullptr) SM_Chest_Top->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY);
	SetActorTickEnabled(true);
	Swap(MaxAngle, ToggleAngle);
	Angle = ToggleAngle;

	if (MaxAngle > 0.f)
	{
		RectLight->SetIntensity(0.f);
	}
	else
	{
		RectLight->SetIntensity(1000.f);
	}
	if (PlacedStage != nullptr) PlacedStage->SetChestOpenState(PlacedCoordinate, MaxAngle == 67.f ? INTERACTABLE_ITEM_STATE_CLOSED : INTERACTABLE_ITEM_STATE_OPEN_L);
}

