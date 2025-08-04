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
#include "Interfaces/IEquipment.h"
#include "Interfaces/IInteractableItem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/UI/CStatusStage.h"
#include "Player/UI/CItemTooltipWidget.h"
#include "Player/UI/CDamage.h"


ACPlayerController::ACPlayerController() : Super()
{
	//PlayerCameraManagerClass = ACPlayerCameraManager::StaticClass();

	ConstructorHelpers::FClassFinder<UUserWidget> HUDFinder(TEXT("/Game/UI/BP_HUD"));
	if (HUDFinder.Succeeded()) HUDClass = HUDFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> InventoryFinder(TEXT("/Game/UI/BP_Inventory"));
	if (InventoryFinder.Succeeded()) InventoryClass = InventoryFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> WidemapFinder(TEXT("/Game/UI/BP_Widemap"));
	if (WidemapFinder.Succeeded()) WidemapClass = WidemapFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> ItemTooltipFinder(TEXT("/Game/UI/BP_ItemTooltip"));
	if (ItemTooltipFinder.Succeeded()) ItemTooltipWidgetClass = ItemTooltipFinder.Class;
}

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Data
	GameModeDataManager = Cast<IIGameModeDataManager>(GetWorld()->GetAuthGameMode());
	PlayerState = Cast<IIPlayerState>(GetPlayerState<IIPlayerState>());
	ObjectPoolManager = Cast<IIObjectPoolManager>(GetWorld()->GetGameState());
	PlayerCharacterStage = Cast<IIPlayerOnStage>(GetCharacter());

	StatusStage = Cast<ACStatusStage>(UGameplayStatics::GetActorOfClass(GetWorld(), ACStatusStage::StaticClass()));
	if (StatusStage)
	{
		UE_LOG(LogTemp, Log, TEXT("ACPlayerController: Found ACStatusStage in level."));
		if (PlayerCharacterStage)
		{
			StatusStage->SetupDelegates(PlayerCharacterStage->GetOnEquipmentChangedDelegate());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACPlayerController: ACStatusStage not found in level."));
	}

	// UI
	Widget_HUD = CreateWidget<UCHUD>(this, HUDClass);
	Widget_Inventory = CreateWidget<UCInventory>(this, InventoryClass);
	Widget_Widemap = CreateWidget<UCWidemap>(this, WidemapClass);
	Widget_ItemTooltip = CreateWidget<UCItemTooltipWidget>(this, ItemTooltipWidgetClass);
	Widget_ItemTooltip_Additional = CreateWidget<UCItemTooltipWidget>(this, ItemTooltipWidgetClass);

	if (Widget_ItemTooltip != nullptr)
	{
		Widget_ItemTooltip->AddToViewport(100);
		Widget_ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Widget_ItemTooltip_Additional != nullptr)
	{
		Widget_ItemTooltip_Additional->AddToViewport(100);
		Widget_ItemTooltip_Additional->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Data
	if (Widget_Inventory != nullptr && PlayerState != nullptr)
	{
		// Execute When Add New Item (Not Count++)
		PlayerState->SetUIInventoryDelegate(Widget_Inventory->GetDelegate_InsertItem());
		PlayerState->SetEquipDelegates(&Delegate_EquipItem, &Delegate_UnEquipItem);
		Widget_Inventory->SetDelegates(
			&OnItemHoveredDelegate,
			&OnItemUnhoveredDelegate,
			&Delegate_EquipItem,
			&Delegate_UnEquipItem,
			PlayerState->GetOnStatusChangedDelegate(),
			PlayerState->GetOnInventoryChangedDelegate(),
			&Widget_HUD->OnQuickslotChanged
		);
	}

	OnItemHoveredDelegate.BindUFunction(this, FName("ShowItemTooltip"));
	OnItemUnhoveredDelegate.BindUFunction(this, FName("HideItemTooltip"));
	Delegate_EquipItem.AddUFunction(this, FName("EquipItem"));
	Delegate_UnEquipItem.AddUFunction(this, FName("UnEquipItem"));

	// UI
	if (Widget_HUD != nullptr)
	{
		Widget_HUD->AddToViewport();
		if (PlayerState != nullptr)
		{
			Widget_HUD->SetupParameterDelegates(
				PlayerState->GetOnStatusChangedDelegate()
			);
			PlayerState->SetupDelegates(&OnChangeCounterReady, &Delegate_OutOfMana);
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
	if (PlayerState != nullptr)
	{
		PlayerState->SetMaxHP(100.f);
		PlayerState->SetHP(100.f);
	}
}

void ACPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Widget_HUD != nullptr) Widget_HUD->SetMinimapAngle(-GetControlRotation().Yaw - 90.f);
	if (Widget_Widemap != nullptr) Widget_Widemap->SetMinimapAngle(-GetControlRotation().Yaw - 90.f);
	if (Widget_ItemTooltip != nullptr && Widget_ItemTooltip->GetVisibility() == ESlateVisibility::Visible)
	{
		float MouseX, MouseY;
		if (GetMousePosition(MouseX, MouseY))
		{
			Widget_ItemTooltip->SetPositionInViewport(FVector2D(MouseX, MouseY) + FVector2D(10.f, 10.f), true);
			if (Widget_ItemTooltip_Additional != nullptr && Widget_ItemTooltip_Additional->GetVisibility() == ESlateVisibility::Visible)
			{
				FVector2D TooltipSize = Widget_ItemTooltip->GetDesiredSize();
				Widget_ItemTooltip_Additional->SetPositionInViewport(FVector2D(MouseX, MouseY) + FVector2D(-TooltipSize.X * 0.9f, 10.f), true);
			}
		}
	}

	if (!DamageComponentQueue.IsEmpty())
	{
		UCDamage* DamageComponent;
		DamageComponentQueue.Peek(DamageComponent);
		if (DamageComponent != nullptr && DamageComponent->SpawnStamp + 1.f < GetWorld()->TimeSeconds)
		{
			DamageComponentQueue.Dequeue(DamageComponent);
			ObjectPoolManager->ReturnDamageComponent(DamageComponent);
		}
	}
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
	if (Widget_Inventory == nullptr) return;

	if (Widget_Inventory->IsDisabled()) return;

	if (Widget_Inventory->GetVisibility() != ESlateVisibility::Visible)
	{
		if (StatusStage != nullptr)
		{
			SetShowMouseCursor(true);
			StatusStage->ActivateStageCamera(this, 0.0f);
			Widget_Inventory->SetVisibility(ESlateVisibility::Visible);
			Widget_HUD->SetVisibility(ESlateVisibility::Collapsed);
			//ToggleMinimap(false);
		}
	}
	else
	{
		if (StatusStage != nullptr && GetPawn() != nullptr)
		{
			SetShowMouseCursor(false)	;
			StatusStage->DeactivateStageCamera(this, GetPawn(), 0.5f);
			Widget_Inventory->SetVisibility(ESlateVisibility::Collapsed);
			Widget_HUD->SetVisibility(ESlateVisibility::Visible);
			//ToggleMinimap(true);
		}
	}
}

bool ACPlayerController::InsertItem(UCItemData* ItemData, AActor* Item)
{
	int32 ItemCategory = ItemData->ItemCategory;
	int32 ItemType = ItemData->ItemCode;
	UCItemData* ModDataAsset = nullptr;
	if (Widget_Inventory == nullptr && PlayerState == nullptr) return false;
	if (PlayerState->InsertItem(ItemData, ModDataAsset)) // Data Insert succeeded
	{
		switch (ItemCategory)
		{
		case(ITEM_CATEGORY_DISPOSABLE):
			ObjectPoolManager->ReturnItem(Item, ItemType);
			break;
		case(ITEM_CATEGORY_EQUIPMENT):
			ObjectPoolManager->ReturnEquipment(Cast<IIEquipment>(Item), ItemType);
			break;
		default:
			break;
		}
		return true;
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

bool ACPlayerController::IsInventoryVisible()
{
	if (Widget_Inventory == nullptr) return false;
	return Widget_Inventory->GetVisibility() == ESlateVisibility::Visible;
}

void ACPlayerController::ShowItemTooltip(UCItemData* ItemData)
{
	if (Widget_ItemTooltip == nullptr) return;

	float MouseX, MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		UCItemData* EquippedItemData = PlayerState->GetEquippedItemData(ItemData->ItemEquipSlot);
		bool DoCompareAbility = ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT && !ItemData->Equipped;
		Widget_ItemTooltip->SetItemData(ItemData, (DoCompareAbility && Widget_ItemTooltip_Additional != nullptr) ? EquippedItemData : nullptr);
		Widget_ItemTooltip->SetVisibility(ESlateVisibility::Visible);
		Widget_ItemTooltip->SetPositionInViewport(FVector2D(MouseX, MouseY) + FVector2D(10.f, 10.f), true);

		if (
			(Widget_ItemTooltip_Additional != nullptr && EquippedItemData != nullptr) &&
			DoCompareAbility
			)
		{	
			Widget_ItemTooltip_Additional->SetItemData(EquippedItemData);
			Widget_ItemTooltip_Additional->SetVisibility(ESlateVisibility::Visible);
			FVector2D TooltipSize = Widget_ItemTooltip->GetDesiredSize();
			Widget_ItemTooltip_Additional->SetPositionInViewport(FVector2D(MouseX, MouseY) + FVector2D(-TooltipSize.X / 2.f + 10.f, 10.f), true);
		}
	}
}

void ACPlayerController::HideItemTooltip()
{
	if (Widget_ItemTooltip == nullptr) return;
	Widget_ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);

	if (Widget_ItemTooltip_Additional != nullptr)
	{
		Widget_ItemTooltip_Additional->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ACPlayerController::EquipItem(UCItemData* ItemData)
{
	if (ItemData == nullptr) return;
	if (PlayerCharacterStage == nullptr) return;
	if (ObjectPoolManager == nullptr) return;

	if (ItemData->ItemCategory == ITEM_CATEGORY_DISPOSABLE)	EquipItem_Disposable(ItemData);
	if (ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT)	EquipItem_Equipment(ItemData);
}

void ACPlayerController::EquipItem_Equipment(UCItemData* ItemData)
{
	UCItemData* E_ItemData = PlayerState->GetEquippedItemData(ItemData->ItemEquipSlot);
	if (E_ItemData != nullptr)
	{
		Delegate_UnEquipItem.Broadcast(E_ItemData);
	}

	// Spawn & Attach To Level
	IIEquipment* ToEquipEquipment = ObjectPoolManager->GetEquipment(this, ItemData->ItemCode, FTransform(PlayerCharacterStage->GetLocation()));
	AActor* EquippedActor = Cast<AActor>(ToEquipEquipment);
	if (EquippedActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACPlayerController::EquipItem: Failed to get equipment actor from pool."));
		return;
	}
	ToEquipEquipment->Equip(true);
	Cast<IIInteractableItem>(EquippedActor)->SetItemData(ItemData);
	if (ItemData->ItemEquipSlot == EQUIPMENT_SWORD)
	{
		PlayerCharacterStage->AttachEquipment(EquippedActor, ItemData->ItemEquipSlot, PLAYER_SOCKET_WEAPON_R);
	}
	else
	{
		PlayerCharacterStage->AttachEquipment(ToEquipEquipment, ItemData->ItemEquipSlot);
	}
}

void ACPlayerController::EquipItem_Disposable(UCItemData* ItemData)
{
	if (ItemData->Equipped) return;
	ItemData->Equipped = true;
	ItemData->Quickslot = Widget_Inventory->GetEmptyQuickSlotIndex();

	UE_LOG(LogTemp, Warning, TEXT("ACPlayerController::EquipItem: Failed to get equipment actor from pool."));
}

void ACPlayerController::UnEquipItem(UCItemData* ItemData)
{
	if (ItemData == nullptr) return;
	if (PlayerCharacterStage == nullptr) return;
	if (ItemData->ItemCategory == ITEM_CATEGORY_EQUIPMENT)
	{
		IIEquipment* DetachedEquipment = PlayerCharacterStage->DetachEquipment(ItemData->ItemEquipSlot);

		if (DetachedEquipment != nullptr && ObjectPoolManager != nullptr)
		{
			ObjectPoolManager->ReturnEquipment(DetachedEquipment, ItemData->ItemCode);
		}
	}
	else if (ItemData->ItemCategory == ITEM_CATEGORY_DISPOSABLE)
	{
		ItemData->Equipped = false;
	}
}

bool ACPlayerController::GetHasWeapon()
{
	if (PlayerState == nullptr) return false;
	return PlayerState->GetHasWeapon();
}

void ACPlayerController::SetHasWeapon(bool bHasWeapon)
{
	if (PlayerState != nullptr)
	{
		PlayerState->SetHasWeapon(bHasWeapon);
	}
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
	FPlayerStat CurrStat = GetPlayerStat();

	bool bIsCauserFacingPlayer = false;
	if (DamageConfig.Causer != nullptr && PlayerCharacterStage != nullptr)
	{
		FVector CauserToPlayerDirection = (DamageConfig.Causer->GetActorLocation() - PlayerCharacterStage->GetLocation()).GetSafeNormal();
		FVector PlayerForwardVector = PlayerCharacterStage->GetForwardVector();
		//DrawDebugDirectionalArrow(GetWorld(), PlayerCharacterStage->GetLocation(), PlayerCharacterStage->GetLocation() + PlayerForwardVector * 100.f, 50.f, FColor::Red, false, 3.f);
		//DrawDebugDirectionalArrow(GetWorld(), PlayerCharacterStage->GetLocation(), PlayerCharacterStage->GetLocation() + CauserToPlayerDirection * 100.f, 50.f, FColor::Blue, false, 3.f);
		if (FVector::DotProduct(PlayerForwardVector, CauserToPlayerDirection) > 0.7f)
		{
			bIsCauserFacingPlayer = true;
		}
	}

	if (bCounterHitCheck && bIsCauserFacingPlayer && ObjectPoolManager != nullptr)
	{
		if (DamageConfig.AttackType == ATTACK_TYPE_MELLE)
		{
			FRotator SpawnEffectRotator;
			if (DamageConfig.Causer != nullptr) SpawnEffectRotator = (DamageConfig.Causer->GetActorLocation() - PlayerCharacterStage->GetLocation()).GetSafeNormal2D().Rotation();
			else SpawnEffectRotator = PlayerCharacterStage->GetRotation();
			ObjectPoolManager->SpawnParticle(
				nullptr, NAME_None, PARTICLE_PLAYER_HIT_COUNTER_SUCCEEDED, FTransform(
					SpawnEffectRotator - FRotator(90.f, 0.f, 0.f), GetPlayerLocation(), FVector(1.f)
				)
			);
			PlayerCharacterStage->CounterAttackSucceeded(DamageConfig);
			SetCounterHitCheck(false);
		}
		else if (DamageConfig.AttackType == ATTACK_TYPE_RANGED || DamageConfig.AttackType == ATTACK_TYPE_MAGIC)
		{

		}
		return false;
	}

	if (PlayerState != nullptr)
	{
		float DefRate = (100.f - CurrStat.DefencePower / 20.f) / 100.f;
		DamageConfig.Damage *= DefRate;
		PlayerState->RecieveDamage(DamageConfig.Damage);
	}
	if (ObjectPoolManager != nullptr)
	{
		ObjectPoolManager->SpawnParticle(
			nullptr, NAME_None, DamageConfig.HitParticleType, FTransform(
				FRotator::ZeroRotator, DamageConfig.HitLocation, FVector(1.f)
			)
		);
		UCDamage* DamageComponent = ObjectPoolManager->GetDamageComponent(GetCharacter(), DamageConfig);
		DamageComponent->SetController(this);
		DamageComponentQueue.Enqueue(DamageComponent);
		if (DamageConfig.Instigator != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ACPlayerController::RecieveDamage : Instigator is %s"), *DamageConfig.Instigator->GetName());
		}
		if (DamageConfig.Causer != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ACPlayerController::RecieveDamage : Causer is %s"), *DamageConfig.Causer->GetName());
		}
	}
	return false;
}

bool ACPlayerController::DealtDamage(FDamageConfig DamageConfig)
{
	if (ObjectPoolManager != nullptr)
	{
		UCDamage* DamageComponent = ObjectPoolManager->GetDamageComponent(GetCharacter(), DamageConfig);
		DamageComponent->SetController(this);
		DamageComponentQueue.Enqueue(DamageComponent);
		if (DamageConfig.Instigator != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ACPlayerController::RecieveDamage : Instigator is %s"), *DamageConfig.Instigator->GetName());
		}
		if (DamageConfig.Causer != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ACPlayerController::RecieveDamage : Causer is %s"), *DamageConfig.Causer->GetName());
		}
		return true;
	}
	return false;
}

bool ACPlayerController::AttachEquipment(IIEquipment* ToEquipItem, int32 Type, FName SocketName)
{
	AActor* tempActor = Cast<AActor>(ToEquipItem);
	return PlayerCharacterStage->AttachEquipment(tempActor, Type, SocketName);
}

IIObjectPoolManager* ACPlayerController::GetObjectPoolManager()
{
	return ObjectPoolManager;
}

FRotator ACPlayerController::GetCurrentCameraRotation()
{
	if (PlayerCharacterStage != nullptr) return PlayerCharacterStage->GetCameraTransform().GetRotation().Rotator();
	return FRotator();
}

void ACPlayerController::LockOnMonster(IIEnemyCharacter* Enemy)
{
	if (PlayerCharacterStage != nullptr && Enemy != nullptr) PlayerCharacterStage->LockOnMonster(Enemy);
}

void ACPlayerController::LockFreeMonster()
{
	if (PlayerCharacterStage != nullptr) PlayerCharacterStage->LockFreeMonster();
}

FPlayerStat ACPlayerController::GetPlayerStat()
{
	if (PlayerState != nullptr) return PlayerState->GetPlayerStat();
	return FPlayerStat();
}

void ACPlayerController::SetCounterHitCheck(bool b)
{
	bCounterHitCheck = b;
}

bool ACPlayerController::GetCounterHitCheck()
{
	return bCounterHitCheck;
}

FOnChangeCounterReady* ACPlayerController::GetOnChangeCounterReadyDelegate()
{
	return &OnChangeCounterReady;
}

bool ACPlayerController::TrySpendMP(float e)
{
	if (PlayerState == nullptr || PlayerState->GetMP() <= 0.f) return false;
	PlayerState->SetMP(PlayerState->GetMP() - e);
	return true;
}

float ACPlayerController::GetCurrentMP()
{
	if (PlayerState != nullptr) return PlayerState->GetMP();
	return 0.0f;
}

//void ACPlayerController::SetupDelegates(FMONTAGE_PLAYING_STATE_CHANGED* Delegate_MontagePlayingStateChanged)
//{
//
//}

