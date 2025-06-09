#include "CPlayerController.h"
#include "CHUD.h"
#include "CInventory.h"
#include "CWidemap.h"
#include "GameFrameWork/GameModeBase.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "CGameState_Stage.h"
#include "IHUD.h"
#include "IStageBuild.h"
#include "IGeneratedStage.h"
#include "IGameModeDataManager.h"
#include "IObjectPoolManager.h"
#include "IPlayerState.h"
#include "IUIInventory.h"

ACPlayerController::ACPlayerController() : Super()
{
	ConstructorHelpers::FClassFinder<UUserWidget> HUDFinder(TEXT("/Game/UI/BP_HUD"));
	if (HUDFinder.Succeeded()) HUDClass = HUDFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> InventoryFinder(TEXT("/Game/UI/BP_Inventory"));
	if (InventoryFinder.Succeeded()) InventoryClass = InventoryFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> WidemapFinder(TEXT("/Game/UI/BP_Widemap"));
	if (WidemapFinder.Succeeded()) WidemapClass = WidemapFinder.Class;

}

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();
	Widget_HUD = CreateWidget<UCHUD>(this, HUDClass);
	Widget_Inventory = CreateWidget<UCInventory>(this, InventoryClass);
	Widget_Widemap = CreateWidget<UCWidemap>(this, WidemapClass);

	if (Widget_HUD != nullptr) Widget_HUD->AddToViewport();
	if (Widget_Inventory != nullptr)
	{
		Widget_Inventory->AddToViewport();
		Widget_Inventory->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Widget_Widemap != nullptr)
	{
		Widget_Widemap->AddToViewport();
		Widget_Widemap->SetVisibility(ESlateVisibility::Collapsed);
	}
	GameModeDataManager = Cast<IIGameModeDataManager>(GetWorld()->GetAuthGameMode());
	PlayerState = Cast<IIPlayerState>(GetPlayerState<IIPlayerState>());
	ObjectPoolManager = Cast<IIObjectPoolManager>(GetWorld()->GetGameState());

	IIUIInventory* UIInventory = Cast<IIUIInventory>(Widget_Inventory);
	if (Widget_Inventory != nullptr && PlayerState != nullptr)
	{
		// Execute When Add New Item (Not Count++)
		PlayerState->SetUIInventoryDelegate(Widget_Inventory->GetDelegate_InsertItem());
	}
}

void ACPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Widget_HUD != nullptr) Widget_HUD->SetMinimapAngle(-GetControlRotation().Yaw - 90.f);
	if (Widget_Widemap != nullptr) Widget_Widemap->SetMinimapAngle(-GetControlRotation().Yaw - 90.f);
}

IIHUD* ACPlayerController::GetWidemapInterface()
{
	if (Widget_Widemap != nullptr) return Cast<IIHUD>(Widget_Widemap);
	else UE_LOG(LogTemp, Error, TEXT("ACPlayerController : GetWidemapInterface : Could Not Convert Widget"));
	return nullptr;
}

IIHUD* ACPlayerController::GetHUDInterface()
{
	if (Widget_HUD != nullptr) return Cast<IIHUD>(Widget_HUD);
	return nullptr;
}

void ACPlayerController::ToggleInventory()
{
	if (Widget_Inventory->GetVisibility() != ESlateVisibility::Visible) Widget_Inventory->SetVisibility(ESlateVisibility::Visible);
	else Widget_Inventory->SetVisibility(ESlateVisibility::Collapsed);

}

void ACPlayerController::InsertItem(FINSERT_ITEM*& Delegate_InsertItem, AActor* Item, int32 ItemType)
{
	if (Widget_Inventory != nullptr && PlayerState != nullptr)
	{
		PlayerState->GetInventoryDelegate(Delegate_InsertItem);
		ObjectPoolManager->ReturnItem(Item, ItemType);
		Widget_Inventory->Refresh_ItemTile();
	}
}

void ACPlayerController::GetInventoryDelegate(FINSERT_ITEM*& Delegate_InsertItem)
{
	if (Widget_Inventory != nullptr && PlayerState != nullptr)
	{
		PlayerState->GetInventoryDelegate(Delegate_InsertItem);
	}
}

void ACPlayerController::ToggleWidemap(bool e)
{
	if (Widget_Widemap != nullptr) Widget_Widemap->SetVisibility(e ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void ACPlayerController::ToggleMinimap(bool e)
{
	Widget_HUD->ToggleMinimap(e);
}

void ACPlayerController::MinimapRemoveBind()
{
	if (CurrentStage != nullptr) CurrentStage->MinimapRemoveBind();
}

void ACPlayerController::LightsOutPrevStage()
{
	if (PreviousStage != nullptr)
	{
		PreviousStage->LightsOut();
		PreviousStage->Stage_GridReturn();
		PreviousStage = nullptr;
	}
}

void ACPlayerController::SetStage(IIGeneratedStage* Stage)
{
	PreviousStage = CurrentStage;
	CurrentStage = Stage;
}

void ACPlayerController::SightReached(FVector& SightLocation)
{
	if (CurrentStage!= nullptr) CurrentStage->PlayerSightReached(SightLocation);
}

FVector ACPlayerController::GetPlayerLocation()
{
	return (GetPawn() != nullptr ? GetPawn()->GetActorLocation() : FVector::ZeroVector);
}
