#include "CDoor.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IGeneratedStage.h"

ACDoor::ACDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SMC_DoorStamp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_DoorStamp"));
	SMC_Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Door"));

	SetRootComponent(SceneComponent);
	SMC_DoorStamp->SetupAttachment(SceneComponent);
	SMC_Door->SetupAttachment(SceneComponent);
	SMC_Door->SetRelativeLocation(FVector(60.f, 25.f, 0.f));

	ConstructorHelpers::FObjectFinder<UStaticMesh> DoorStampFinder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gate-01-01.SM-Gate-01-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Door1Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gate-01-02.SM-Gate-01-02"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Door2Finder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Gate-01-03.SM-Gate-01-03"));

	if (SMC_DoorStamp != nullptr && DoorStampFinder.Succeeded()) SMC_DoorStamp->SetStaticMesh(DoorStampFinder.Object);
	if (FMath::FRandRange(0.f, 1.f) < 0.5f)
	{
		if (SMC_Door != nullptr && Door1Finder.Succeeded()) SMC_Door->SetStaticMesh(Door1Finder.Object);
	}
	else
	{
		if (SMC_Door != nullptr && Door2Finder.Succeeded()) SMC_Door->SetStaticMesh(Door2Finder.Object);
	}
	SMC_Door->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SMC_DoorStamp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SMC_Door->SetCollisionResponseToChannel(COLLISION_CHANNEL_PLAYER_GAZE, ECollisionResponse::ECR_Block);
	SMC_DoorStamp->SetCollisionResponseToChannel(COLLISION_CHANNEL_PLAYER_GAZE, ECollisionResponse::ECR_Block);
}

void ACDoor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void ACDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bLocked)
	{
		if (TargetAngle != 0.f)
		{
			Angle += TargetAngle * DeltaTime;
			if (TargetAngle < 0.f && Angle <= TargetAngle / 30.f ||
				TargetAngle > 0.f && Angle >= TargetAngle / 30.f)
			{
				TargetAngle = 0.f;
			}
			SMC_Door->SetRelativeRotation(FRotator(0.f, Angle, 0.f));
		}
		else
		{
			bool Flag = Angle > 0.f ? true : false;
			Angle += (Flag ? -90.f : 90.f) * DeltaTime;
			if ((Flag && Angle <= 0.f) || (!Flag && Angle >= 0.f))
			{
				Angle = TargetAngle;
				bBusy = false;
				SetActorTickEnabled(false);
			}
			SMC_Door->SetRelativeRotation(FRotator(0.f, Angle, 0.f));
		}
	}
	else if (TargetAngle != 0.f)
	{
		Angle += TargetAngle * DeltaTime;
		if (TargetAngle < 0.f && Angle <= TargetAngle ||
			TargetAngle > 0.f && Angle >= TargetAngle)
		{
			Angle = TargetAngle;
			bBusy = false;
			SetActorTickEnabled(false);
			if (SMC_DoorStamp != nullptr) SMC_DoorStamp->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
			if (SMC_Door != nullptr) SMC_Door->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
		}
		if (SMC_Door != nullptr) SMC_Door->SetRelativeRotation(FRotator(0.f, Angle, 0.f));
	}
	else
	{
		bool Flag = Angle > 0.f ? true : false;
		Angle += (Flag ? -90.f : 90.f) * DeltaTime;
		if ((Flag && Angle <= 0.f) || (!Flag && Angle >= 0.f))
		{
			Angle = TargetAngle;
			bBusy = false;
			SetActorTickEnabled(false);
			if (SMC_DoorStamp != nullptr) SMC_DoorStamp->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
			if (SMC_Door != nullptr) SMC_Door->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM);
		}
		if (SMC_Door != nullptr) SMC_Door->SetRelativeRotation(FRotator(0.f, Angle, 0.f));
	}
}

void ACDoor::Select()
{
 	if (Selected) return;
	Selected = true;
	if (SMC_Door != nullptr)
	{
		SMC_Door->SetRenderCustomDepth(true);
		SMC_Door->SetCustomDepthStencilValue((bBusy || bLocked) ? CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY : CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
	if (SMC_DoorStamp != nullptr)
	{
		SMC_DoorStamp->SetRenderCustomDepth(true);
		SMC_DoorStamp->SetCustomDepthStencilValue((bBusy || bLocked) ? CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY : CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
}

void ACDoor::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (SMC_Door != nullptr)
	{
		SMC_Door->SetRenderCustomDepth(false);
	}
	if (SMC_DoorStamp != nullptr)
	{
		SMC_DoorStamp->SetRenderCustomDepth(false);
	}
}

void ACDoor::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	if (bBusy) return;
	UnSelect();
	bBusy = true;
	SetActorTickEnabled(true);
	if (SMC_DoorStamp != nullptr) SMC_DoorStamp->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY);
	if (SMC_Door != nullptr) SMC_Door->SetCustomDepthStencilValue(CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY);

	FVector CurrLocation = GetActorLocation();
	FVector PlayerDir = (GetActorLocation() - (PlayerControllerStage != nullptr ? PlayerControllerStage->GetPlayerLocation() : FVector::ZeroVector)).GetSafeNormal2D();
	FVector DoorRDir = GetActorRightVector();
	FVector DoorLDir = -GetActorRightVector();
	float RRad = FMath::Acos(FVector::DotProduct(PlayerDir, DoorRDir));
	float LRad = FMath::Acos(FVector::DotProduct(PlayerDir, DoorLDir));

	if (TargetAngle == 0.f)
	{
		TargetAngle = RRad < LRad ? RMaxAngle : LMaxAngle;
		if (PlacedStage != nullptr) PlacedStage->SetDoorOpenState(PlacedCoordinate, RRad < LRad ? INTERACTABLE_ITEM_STATE_OPEN_R : INTERACTABLE_ITEM_STATE_OPEN_L, PlacedIndex);
	}
	else
	{
		TargetAngle = 0.f;
		if (PlacedStage != nullptr) PlacedStage->SetDoorOpenState(PlacedCoordinate, INTERACTABLE_ITEM_STATE_CLOSED, PlacedIndex);
	}
}

void ACDoor::SetVisibility(bool e)
{
	if (e)
	{
		SceneComponent->Activate();
		SMC_DoorStamp->Activate();
		SMC_Door->Activate();
	}
	else
	{
		if (TargetAngle != 0.f)
		{
			TargetAngle = 0.f;
			Angle = 0.f;
			SMC_Door->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		}
		SceneComponent->Deactivate();
		SMC_DoorStamp->Deactivate();
		SMC_Door->Deactivate();
	}
	SetActorEnableCollision(e);
	if (SMC_Door != nullptr)
	{
		SMC_Door->SetVisibility(e);
		SMC_Door->SetHiddenInGame(!e);
	}
	if (SMC_DoorStamp != nullptr)
	{
		SMC_DoorStamp->SetVisibility(e);
		SMC_DoorStamp->SetHiddenInGame(!e);
	}
	UnSelect();
}

bool ACDoor::GetVisibility()
{
	return (SMC_DoorStamp->IsActive() && SMC_Door->IsActive() ? true : false);
}

void ACDoor::ManualInteract(int32 OpenState, bool bForce)
{
	if (bForce)
	{
		TargetAngle = (OpenState == INTERACTABLE_ITEM_STATE_CLOSED ? 0.f : (OpenState == INTERACTABLE_ITEM_STATE_OPEN_L ? LMaxAngle : RMaxAngle));
		Angle = (OpenState == INTERACTABLE_ITEM_STATE_CLOSED ? 0.f : (OpenState == INTERACTABLE_ITEM_STATE_OPEN_L ? LMaxAngle : RMaxAngle));
		SMC_Door->SetRelativeRotation(FRotator(0.f, Angle, 0.f));
	}
	else
	{
		if (OpenState == INTERACTABLE_ITEM_STATE_CLOSED)
		{
			if (Angle != 0.f) Interact();
		}
		else
		{
			if (Angle == 0.f) Interact();
		}
	}
}

