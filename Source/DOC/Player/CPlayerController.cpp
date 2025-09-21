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
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IPlayerState.h"
#include "Interfaces/IUIInventory.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IEquipment.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/IDamagable.h"
#include "Kismet/GameplayStatics.h"
#include "Player/UI/CStatusStage.h"
#include "Player/UI/CItemTooltipWidget.h"
#include "Player/UI/CDamage.h"
#include "Player/UI/CChestItemWidget.h"
#include "Player/UI/CStageClearItemWidget.h"
#include "Player/UI/CBossHPWidget.h"


ACPlayerController::ACPlayerController() : Super()
{
	ConstructorHelpers::FClassFinder<UUserWidget> HUDFinder(TEXT("/Game/UI/BP_HUD"));
	if (HUDFinder.Succeeded()) HUDClass = HUDFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> InventoryFinder(TEXT("/Game/UI/BP_Inventory"));
	if (InventoryFinder.Succeeded()) InventoryClass = InventoryFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> WidemapFinder(TEXT("/Game/UI/BP_Widemap"));
	if (WidemapFinder.Succeeded()) WidemapClass = WidemapFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> ItemTooltipFinder(TEXT("/Game/UI/BP_ItemTooltip"));
	if (ItemTooltipFinder.Succeeded()) ItemTooltipWidgetClass = ItemTooltipFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> ChestItemWidgetFinder(TEXT("/Game/UI/BP_ChsetItem"));
	if (ChestItemWidgetFinder.Succeeded()) ChestItemWidgetClass = ChestItemWidgetFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> StageClearItemWidgetFinder(TEXT("/Game/UI/BP_StageCleartem"));
	if (StageClearItemWidgetFinder.Succeeded()) StageClearItemWidgetClass = StageClearItemWidgetFinder.Class;
	ConstructorHelpers::FClassFinder<UUserWidget> BossHPWidgetClassFinder(TEXT("/Game/UI/BP_BossHP"));
	if (BossHPWidgetClassFinder.Succeeded()) BossHPWidgetClass = BossHPWidgetClassFinder.Class;
}

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ACGameMode_Stage* GameMode = GetWorld()->GetAuthGameMode<ACGameMode_Stage>();
	if (GameMode)
	{
		StageClearedDelegatePtr = GameMode->GetStageClearedDelegatePtr();
		if (StageClearedDelegatePtr)
		{
			StageClearedDelegatePtr->AddUFunction(this, FName("OnStageCleared"));
		}
	}

	// Data
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
	if (PlayerCharacterStage && PlayerState)
	{
		Delegate_GroggyOnPtr = PlayerState->GetGroggyOnDelegate();
		Delegate_GroggyEndPtr = PlayerState->GetGroggyEndDelegate();
		PlayerCharacterStage->SetupDelegates(Delegate_GroggyOnPtr, Delegate_GroggyEndPtr);
		if (Delegate_GroggyOnPtr != nullptr)
		{
			Delegate_GroggyOnPtr->AddUFunction(this, FName("OnGroggy"));
		}
	}
	// UI
	Widget_HUD = CreateWidget<UCHUD>(this, HUDClass);
	Widget_Inventory = CreateWidget<UCInventory>(this, InventoryClass);
	Widget_Widemap = CreateWidget<UCWidemap>(this, WidemapClass);
	Widget_ItemTooltip = CreateWidget<UCItemTooltipWidget>(this, ItemTooltipWidgetClass);
	Widget_ItemTooltip_Additional = CreateWidget<UCItemTooltipWidget>(this, ItemTooltipWidgetClass);
	Widget_ChestItem = CreateWidget<UCChestItemWidget>(this, ChestItemWidgetClass);
	Widget_StageClearItem = CreateWidget<UCStageClearItemWidget>(this, StageClearItemWidgetClass);
	Widget_BossHP = CreateWidget<UCBossHPWidget>(this, BossHPWidgetClass);

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
	if (Widget_ChestItem != nullptr)
	{
		Widget_ChestItem->AddToViewport(99);
		Widget_ChestItem->SetVisibility(ESlateVisibility::Collapsed);
		Widget_ChestItem->SetDelegates(&OnItemHoveredDelegate, &OnItemUnhoveredDelegate, PlayerState != nullptr ? PlayerState->GetGetItemDelegate() : nullptr);
	}
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
	if (Widget_StageClearItem != nullptr)
	{
		Widget_StageClearItem->AddToViewport(100);
		Widget_StageClearItem->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Widget_BossHP != nullptr)
	{
		Widget_BossHP->AddToViewport(101);
		Widget_BossHP->SetVisibility(ESlateVisibility::Collapsed);
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

void ACPlayerController::OpenChestItemWidget(TArray<class UCItemData*>* ToShowItemData)
{
	Widget_ChestItem->OpenChest(ToShowItemData);
}

void ACPlayerController::CloseChestItemWidget()
{
	Widget_ChestItem->SetVisibility(ESlateVisibility::Collapsed);
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
	if (Widget_ChestItem == nullptr ||
		Widget_ChestItem->GetVisibility() == ESlateVisibility::Visible) return;

	if (Widget_Inventory->GetVisibility() != ESlateVisibility::Visible)
	{
		if (StatusStage != nullptr)
		{
			PlayerState->SortInventoryItems();
			SetShowMouseCursor(true);
			StatusStage->ActivateStageCamera(this, 0.0f);
			Widget_Inventory->SetVisibility(ESlateVisibility::Visible);
			Widget_HUD->SetVisibility(ESlateVisibility::Collapsed);
			//Widget_ChestItem->SetVisibility(ESlateVisibility::Collapsed);
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
	//if (Widget_Inventory != nullptr && PlayerState != nullptr)
	//{
	//	PlayerState->GetInventoryDelegate(Delegate_InsertItem);
	//}
}

void ACPlayerController::ToggleWidemap(bool e)
{
	if (Widget_Widemap != nullptr) Widget_Widemap->SetVisibility(e ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void ACPlayerController::ToggleMinimap(bool e)
{
	if (Widget_HUD != nullptr) Widget_HUD->ToggleMinimap(e);
}

void ACPlayerController::ToggleBossHPBar(bool e, IIDamagable* InBoss)
{
	if (Widget_BossHP != nullptr)
	{
		Widget_BossHP->SetupDelegate(InBoss != nullptr ? InBoss->GetStatusChanagedDelegate() : nullptr);
		Widget_BossHP->SetVisibility(e ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
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

void ACPlayerController::OnStageCleared(UObject* PlayerCharacter, const TArray<class UCSpawnedEnemyData*>& ClearedItems)
{
	if (Widget_StageClearItem != nullptr)
	{
		Widget_StageClearItem->SetVisibility(ESlateVisibility::Visible);
		Widget_StageClearItem->AddEnemiesToList(ClearedItems);
	}
}

void ACPlayerController::OnPressedKeyboard(FKey Key)
{
	if (Key == EKeys::E)
	{
		UE_LOG(LogTemp, Log, TEXT("Keyboard E Pressed"));
	}
}

void ACPlayerController::OnGroggy(FPlayerStat CurrPlayerStat)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(GroggyTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(GroggyTimerHandle, FTimerDelegate::CreateLambda([&]() {
			if (Delegate_GroggyEndPtr != nullptr) Delegate_GroggyEndPtr->Broadcast();
			}),
			1.f / (FMath::Log2(CurrPlayerStat.HealthRegenPower + 1.f) + 1.f),
			false
		);
	}
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
		//PlayerState->RecieveDamage(DamageConfig.Damage);
 		PlayerState->RecieveDamage(DamageConfig);
	}
	if (ObjectPoolManager != nullptr)
	{
		if (DamageConfig.HitSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageConfig.HitSound, DamageConfig.HitLocation);
		}
		if (DamageConfig.HitEffect != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), DamageConfig.HitEffect, FTransform(
					FRotator::ZeroRotator, DamageConfig.HitLocation, FVector(1.f)
				), true, EPSCPoolMethod::AutoRelease);
		}
		else
		{
			ObjectPoolManager->SpawnParticle(
				nullptr, NAME_None, DamageConfig.HitParticleType, FTransform(
					FRotator::ZeroRotator, DamageConfig.HitLocation, FVector(1.f)
				)
			);
		}
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

void ACPlayerController::UseQuickslotItem(int32 QuickslotIndex)
{
	UCItemData* UseItemData = Widget_HUD->GetQuickslotItemData(QuickslotIndex);
	if (UseItemData != nullptr && UseItemData->ItemCount > 0)
	{
		ObjectPoolManager->SpawnParticle(
			nullptr,
			NAME_None,
			UseItemData->ItemUseSpawnParticle,
			FTransform(
				PlayerCharacterStage->GetRotation(),
				PlayerCharacterStage->GetLocation(),
				FVector(1.f)
			)
		);
		UseItemData->ItemCount -= 1;
	}
	if (PlayerState != nullptr) PlayerState->SortInventoryItems();
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

void ACPlayerController::SetupDelegates(FOnReceivedDamage* Delegate_OnReceivedDamage, FOnQuickSlotInput* Delegate_OnQuickSlotInput, FPressedKeyboard* Delegate_PressedKeyboard)
{
	if (Delegate_OnReceivedDamage != nullptr) Delegate_OnReceivedDamage->AddUFunction(this, TEXT("RecieveDamage"));
	if (Delegate_OnQuickSlotInput != nullptr) Delegate_OnQuickSlotInput->AddUFunction(this, TEXT("UseQuickslotItem"));
	Delegate_PressedKeyboardPtr = Delegate_PressedKeyboard;
	if (Delegate_PressedKeyboardPtr != nullptr)
	{
		Delegate_PressedKeyboard->AddUFunction(this, TEXT("OnPressedKeyboard"));
		Widget_ChestItem->SetKeyboardDelegate(Delegate_PressedKeyboard);
		Widget_StageClearItem->SetupDelegates(Delegate_PressedKeyboard, PlayerState != nullptr ? PlayerState->GetGetItemDelegate() : nullptr);
	}
}

//void ACPlayerController::SetupDelegates(FMONTAGE_PLAYING_STATE_CHANGED* Delegate_MontagePlayingStateChanged)
//{
//
//}

