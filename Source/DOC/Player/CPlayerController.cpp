#include "CPlayerController.h"
#include "Player/UI/CHUD.h"
#include "Player/UI/CInventory.h"
#include "Player/UI/CWidemap.h"
#include "GameFrameWork/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "GameSystem/CGameMode_Stage.h"
#include "GameFramework/Character.h"
#include "Interfaces/IHUD.h"
#include "Interfaces/IStageBuild.h"
#include "Interfaces/IGeneratedStage.h"
#include "Interfaces/IGameModeDataManager.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IPlayerState.h"
#include "Interfaces/IUIInventory.h"
#include "Interfaces/IPlayerOnStage.h"
//#include "Player/CPlayerCameraManager.h"


ACPlayerController::ACPlayerController() : Super()
{
	//PlayerCameraManagerClass = ACPlayerCameraManager::StaticClass();

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

	// Data
	GameModeDataManager = Cast<IIGameModeDataManager>(GetWorld()->GetAuthGameMode());
	PlayerState = Cast<IIPlayerState>(GetPlayerState<IIPlayerState>());
	ObjectPoolManager = Cast<IIObjectPoolManager>(GetWorld()->GetGameState());
	PlayerCharacterStage = Cast<IIPlayerOnStage>(GetCharacter());

	// UI
	Widget_HUD = CreateWidget<UCHUD>(this, HUDClass);
	Widget_Inventory = CreateWidget<UCInventory>(this, InventoryClass);
	Widget_Widemap = CreateWidget<UCWidemap>(this, WidemapClass);

	// Data
	if (Widget_Inventory != nullptr && PlayerState != nullptr)
	{
		// Execute When Add New Item (Not Count++)
		PlayerState->SetUIInventoryDelegate(Widget_Inventory->GetDelegate_InsertItem());
	}


	// UI
	if (Widget_HUD != nullptr)
	{
		Widget_HUD->AddToViewport();
		if (PlayerState != nullptr)
		{
			Widget_HUD->SetupParameterDelegates(
				PlayerState->GetHPChangedDelegate()
			);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ACPlayerController : BeginPlay : PlayerState Cannot Found"));
		}
	}
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

	// Param
	PlayerState->SetMaxHP(100.f);
	PlayerState->SetHP(100.f);
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

bool ACPlayerController::InsertItem(FINSERT_ITEM*& Delegate_InsertItem, AActor* Item, int32 ItemType)
{
	if (Widget_Inventory != nullptr && PlayerState != nullptr)
	{
		PlayerState->GetInventoryDelegate(Delegate_InsertItem);
		if (Delegate_InsertItem != nullptr && Delegate_InsertItem->IsBound())
		{
			ObjectPoolManager->ReturnItem(Item, ItemType);
			Widget_Inventory->Refresh_ItemTile();
			return true;
		}
	}
	return false;
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

void ACPlayerController::SetToPerspectiveCamera(FTransform Transform)
{
	if (PlayerCharacterStage != nullptr)
	{
		PlayerCharacterStage->SetToPerspectiveCamera(Transform);
		bShowMouseCursor = true;
		bEnableMouseOverEvents = true;
	}

}

void ACPlayerController::SetToFollowCamera()
{
	if (PlayerCharacterStage != nullptr)
	{
		PlayerCharacterStage->SetToFollowCamera();
		bShowMouseCursor = false;
		bEnableMouseOverEvents = false;
	}
}

void ACPlayerController::GetUnderCursor(FHitResult& HitResult)
{
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
}

bool ACPlayerController::RecieveDamage(FDamageConfig DamageConfig)
{
	if (PlayerState != nullptr)
	{
		PlayerState->RecieveDamage(DamageConfig.Damage);
	}
	if (ObjectPoolManager != nullptr)
	{
		ObjectPoolManager->SpawnParticle(
			nullptr, NAME_None, DamageConfig.HitParticleType, FTransform(
				FRotator::ZeroRotator, DamageConfig.HitLocation, FVector(1.f)
			)
		);
	}
	return false;
}

void ACPlayerController::LockOnMonster(IIEnemyCharacter* Enemy)
{
	if (PlayerCharacterStage != nullptr && Enemy != nullptr) PlayerCharacterStage->LockOnMonster(Enemy);
}

void ACPlayerController::LockFreeMonster()
{
	if (PlayerCharacterStage != nullptr) PlayerCharacterStage->LockFreeMonster();
}
