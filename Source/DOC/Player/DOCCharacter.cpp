#include "DOCCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interfaces/IStageBuild.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "Interfaces/IPlayerControllerUI.h"
#include "Interfaces/IInteractableItem.h"
#include "Interfaces/INavSystemManager.h"
#include "Interfaces/CStageStructs.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Interfaces/IAnimInstance.h"
#include "Interfaces/IEquipment.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/CPlayerGazeComponent.h"
#include "GameSystem/CHitBoxComponent.h"
#include "GameSystem/CStatComponent.h"
#include "Player/CHttpComponent.h"
#include "DrawDebugHelpers.h"

ADOCCharacter::ADOCCharacter()
{
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshFinder(TEXT("/Game/QuangPhan/G2_Mercenaries/Meshes/Characters/Combines/SK_LP287_MercA.SK_LP287_MercA"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshFinder(TEXT("/Game/QuangPhan/G2_Mercenaries/Meshes/Characters/Seperates/Females/SK_FHair.SK_FHair"));

	if (SKMeshFinder.Succeeded()) GetMesh()->SetSkeletalMesh(SKMeshFinder.Object);
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -97.f),
		FRotator(0.f, 0.f, 270.f)
	);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0.f, 20.f, 50.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PerspectiveCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PerspectiveCamera"));
	PerspectiveCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PerspectiveCamera->bUsePawnControlRotation = false;
	PerspectiveCamera->SetActive(false);
	PerspectiveCamera->SetAutoActivate(false);

	LockedOnParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LockedOnParticleSystemComponent"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> LockedOnParticleFinder(TEXT("/Game/Dungeon/FX/P_PrimeHelix_Attack.P_PrimeHelix_Attack"));
	if (LockedOnParticleFinder.Succeeded()) LockedOnParticleSystemComponent->SetTemplate(LockedOnParticleFinder.Object);
	LockedOnParticleSystemComponent->SetAutoActivate(false);
	LockedOnParticleSystemComponent->SetupAttachment(GetRootComponent());

	PlayerGazeComponent = CreateDefaultSubobject<UCPlayerGazeComponent>(TEXT("PlayerGazeComponent"));
	HttpComponent = CreateDefaultSubobject<UCHttpComponent>(TEXT("HttpComponent"));

	ConstructorHelpers::FObjectFinder<UAnimSequence> RMB_ATTACK1_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_01.anim_attack_light_01"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> RMB_ATTACK2_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_02.anim_attack_light_02"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> RMB_ATTACK3_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_med_02.anim_attack_med_02"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> LMB_ATTACK1_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_01.anim_attack_light_01"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> LMB_ATTACK2_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_02.anim_attack_light_02"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> LMB_ATTACK3_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_03.anim_attack_light_03"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> COUNTER_READY_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_heavy_startup.anim_attack_heavy_startup"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> COUNTER_IDLE_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_heavy_idle.anim_attack_heavy_idle"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> COUNTER_ATTACK_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_heavy_release.anim_attack_heavy_release"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> COUNTER_RELEASE_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_heavy_endup.anim_attack_heavy_endup"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> EXECUTE_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_heavy_release.anim_attack_heavy_release"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> ROLL_Finder(TEXT("/Game/Player/Animation/Anim/G2_Stand_To_Roll.G2_Stand_To_Roll"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> Flinch_Finder(TEXT("/Game/Player/Animation/Anim/G2_Sword_And_Shield_Impact.G2_Sword_And_Shield_Impact"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> Dash_Finder(TEXT("/Game/QuangPhan/Common/Animations/InPlace/Females/Anim_Jump_End.Anim_Jump_End"));

	AnimSeqArr.SetNum(PLAYER_ANIMATION_SEQUENCE_NUM);
	if (LMB_ATTACK1_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK1]		= (LMB_ATTACK1_Finder.Object);
	if (LMB_ATTACK2_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK2]		= (LMB_ATTACK2_Finder.Object);
	if (LMB_ATTACK3_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK3]		= (LMB_ATTACK3_Finder.Object);
	if (RMB_ATTACK1_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK1]		= (RMB_ATTACK1_Finder.Object);
	if (RMB_ATTACK2_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK2]		= (RMB_ATTACK2_Finder.Object);
	if (RMB_ATTACK3_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_QTE_ATTACK]		= (RMB_ATTACK3_Finder.Object);
	if (COUNTER_READY_Finder.Succeeded())	AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_READY]		= (COUNTER_READY_Finder.Object);
	if (COUNTER_IDLE_Finder.Succeeded())	AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_IDLE]		= (COUNTER_IDLE_Finder.Object);
	if (COUNTER_RELEASE_Finder.Succeeded())	AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_RELEASE]	= (COUNTER_RELEASE_Finder.Object);
	if (COUNTER_ATTACK_Finder.Succeeded())	AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_ATTACK]	= (COUNTER_ATTACK_Finder.Object);
	if (EXECUTE_Finder.Succeeded())			AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_EXECUTE]			= (EXECUTE_Finder.Object);
	if (ROLL_Finder.Succeeded())			AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_ROLL]				= (ROLL_Finder.Object);
	if (Flinch_Finder.Succeeded())			AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_FLINCH]			= (Flinch_Finder.Object);
	if (Dash_Finder.Succeeded())			AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_DASH]				= (Dash_Finder.Object);

	HitBoxComponent = CreateDefaultSubobject<UCHitBoxComponent>(TEXT("HitBoxComponent"));

	StatComponent = CreateDefaultSubobject<UCStatComponent>(TEXT("StatComponent"));

	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
	HairMesh->SetupAttachment(GetMesh());
	HairMesh->SetMasterPoseComponent(GetMesh());

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(GetMesh());

	TorsoMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TorsoMesh"));
	TorsoMesh->SetupAttachment(GetMesh());
	TorsoMesh->SetMasterPoseComponent(GetMesh());

	GauntletsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GauntletsMesh"));
	GauntletsMesh->SetupAttachment(GetMesh());
	GauntletsMesh->SetMasterPoseComponent(GetMesh());

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetMesh());
	LegsMesh->SetMasterPoseComponent(GetMesh());

	BootsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BootsMesh"));
	BootsMesh->SetupAttachment(GetMesh());
	BootsMesh->SetMasterPoseComponent(GetMesh());
}

void ADOCCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	IPCS = Cast<IIPlayerControllerStage>(GetController());
	if (IPCS != nullptr)
	{
		OnChangeCounterReadyDelegate = IPCS->GetOnChangeCounterReadyDelegate();
		IPCS->GetOutOfManaDelegate()->AddLambda([&]() {
			if (AnimInstance != nullptr && AnimInstance->GetCounterReady())
			{
				if (OnChangeCounterReadyDelegate != nullptr) OnChangeCounterReadyDelegate->Broadcast(false);
				AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_RELEASE], 0.75f, 0.75f);
			}
		});
		IPCS->SetupDelegates(&OnReceivedDamage, &OnQuickSlotInputDelegate, &OnPressedKeyboard);
	}
	IPCUI = Cast<IIPlayerControllerUI>(GetController());
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
	if (HairMesh != nullptr)
	{
		HairMesh->SetRenderCustomDepth(true);
		HairMesh->SetCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
	}
	if (HelmetMesh != nullptr)
	{
		HelmetMesh->SetRenderCustomDepth(true);
		HelmetMesh->SetCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
	}
	if (TorsoMesh != nullptr)
	{
		TorsoMesh->SetRenderCustomDepth(true);
		TorsoMesh->SetCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
	}
	if (GauntletsMesh != nullptr)
	{
		GauntletsMesh->SetRenderCustomDepth(true);
		GauntletsMesh->SetCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
	}
	if (LegsMesh != nullptr)
	{
		LegsMesh->SetRenderCustomDepth(true);
		LegsMesh->SetCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
	}
	if (BootsMesh != nullptr)
	{
		BootsMesh->SetRenderCustomDepth(true);
		BootsMesh->SetCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
	}
	PerspectiveCamera->SetActive(false);
	IINavSystemManager* NavManager = Cast<IINavSystemManager>(GetWorld()->GetGameState());
	NavManager->SetNavigationInvoker(this);
	ObjectPoolManager = Cast<IIObjectPoolManager>(GetWorld()->GetGameState());
	if (LockedOnParticleSystemComponent != nullptr) LockedOnParticleSystemComponent->Deactivate();

	if (PlayerGazeComponent != nullptr) PlayerGazeComponent->InitializeProperties(this, FollowCamera, PerspectiveCamera, ObjectPoolManager);
	AnimInstance = Cast<IIAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance != nullptr)
	{
		FMONTAGE_PLAYING_STATE_CHANGED* Delegate_MontagePlayingStateChanged = AnimInstance->GetDelegate_MontagePlayingStateChanged();
		if (Delegate_MontagePlayingStateChanged != nullptr)
		{
			Delegate_MontagePlayingStateChanged->BindLambda([&](bool newState) {
				bBusyMontage = newState;
				}
			);
		}
		FMONTAGE_PLAYER_COMBO_CLEARED* Delegate_MontagePlayerComboCleared = AnimInstance->GetDelegate_MontagePlayerComboCleared();
		if (Delegate_MontagePlayerComboCleared != nullptr)
		{
			Delegate_MontagePlayerComboCleared->BindLambda([&]() {
				LMB_ComboCount = 0;
				RMB_ComboCount = 0;
				}
			);
		}
		AnimInstance->SetupDelegates(OnChangeCounterReadyDelegate, &OnReceivedDamage, nullptr, nullptr);
	}
	GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
}

void ADOCCharacter::StopJumping()
{
	Super::StopJumping();
}

void ADOCCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickCounter++;
	TickCounter %= 1000000000;
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	FVector TraceStartLocation = FollowCamera->GetComponentLocation();
	FVector CamFor = FollowCamera->GetForwardVector();
	CamFor.Z = 0.f;
	FVector TraceEndLocation = TraceStartLocation + CamFor.GetSafeNormal() * 4000.f;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceEndLocation, ECollisionChannel::ECC_Pawn, CollisionQueryParams);
	if (IPCS != nullptr) IPCS->SightReached(HitResult.bBlockingHit ? HitResult.Location : TraceEndLocation);
	//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Red);
	if (PerspectiveCamera->IsActive())
	{
		if (IPCS != nullptr) IPCS->GetUnderCursor(HitResult);
		else return;
	}
	else GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceStartLocation + FollowCamera->GetForwardVector() * 500.f, COLLISION_CHANNEL_PLAYER_GAZE, CollisionQueryParams);

	IIInteractableItem* tempItem = Cast<IIInteractableItem>(HitResult.GetActor());
	if (HitResult.GetActor() != nullptr) GEngine->AddOnScreenDebugMessage(1, DeltaSeconds, FColor::Green, FString::Printf(TEXT("Forward Speed : %s"), *HitResult.GetActor()->GetName()));

	if (tempItem != nullptr)
	{
		bool IsSelectable = tempItem->IsSelectable(HitResult.GetComponent());
		if (IsSelectable && InteractableItem == nullptr) // New select
		{
			InteractableItem = tempItem;
			InteractableItem->Select();
		}
		else if (IsSelectable && InteractableItem != tempItem) // Move to New Tracing Object
		{
			InteractableItem->UnSelect();
			InteractableItem = tempItem;
			InteractableItem->Select();
		}
		else if (!IsSelectable && InteractableItem == tempItem) // Not Allowed Part Traced
		{
			InteractableItem->UnSelect();
			InteractableItem = nullptr;
		}
		else if (IsSelectable && InteractableItem == tempItem) // Nothing
		{
			//InteractableItem->Select();
		}
		else
		{
			if (InteractableItem != nullptr) InteractableItem->UnSelect();
			InteractableItem = nullptr;
		}
	}
	else // No Tracing Object
	{
		if (InteractableItem != nullptr) InteractableItem->UnSelect();
		InteractableItem = nullptr;
	}

	if (DynamicCameraLocation.Size() > 0.f)
	{
		GetCameraBoom()->SetRelativeLocation(GetCameraBoom()->GetRelativeLocation() + DynamicCameraLocation * DeltaSeconds);
		DynamicCameraLocation -= DynamicCameraLocation * DeltaSeconds;
	}
}

IIPlayerControllerStage* ADOCCharacter::GetPlayerControllerStage()
{
	return GetController() != nullptr ? Cast<IIPlayerControllerStage>(GetController()) : nullptr;
}

UObject* ADOCCharacter::GetControllerAsObject()
{
	return GetController() != nullptr ? GetController() : nullptr;
}

void ADOCCharacter::SetToPerspectiveCamera(FTransform Transform)
{
	if (PerspectiveCamera != nullptr && FollowCamera != nullptr)
	{
		PerspectiveCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		PerspectiveCamera->SetWorldTransform(Transform);
		FollowCamera->SetActive(false);
		PerspectiveCamera->SetActive(true);
	}
}

void ADOCCharacter::SetToFollowCamera()
{
	if (PerspectiveCamera != nullptr && FollowCamera != nullptr)
	{
		PerspectiveCamera->SetActive(false);
		FollowCamera->SetActive(true);
	}
}

void ADOCCharacter::Interact()
{
	if (LockedOnMonster != nullptr && (InteractableItem == nullptr || InteractableItem->_getUObject() != LockedOnMonster->_getUObject()))
	{
		LockFreeMonster();
	}
	OnPressedKeyboard.Broadcast(EKeys::E);
	if (InteractableItem != nullptr) InteractableItem->Interact(IPCUI, IPCS);
	InteractableItem = nullptr;
}

void ADOCCharacter::ToggleInventory()
{
	if (IPCUI != nullptr) IPCUI->ToggleInventory();
}

void ADOCCharacter::TurnOnWidemap()
{
	if (IPCUI != nullptr)
	{
		IPCUI->ToggleWidemap(true);
		IPCUI->ToggleMinimap(false);
	}
}

void ADOCCharacter::TurnOffWidemap()
{
	if (IPCUI != nullptr)
	{
		IPCUI->ToggleWidemap(false);
		IPCUI->ToggleMinimap(true);
	}
}

void ADOCCharacter::LMB()
{
	if (!EquippedActors.Contains(0)) return;
	if (IPCUI != nullptr && IPCUI->IsInventoryVisible()) return;
	if (AnimInstance == nullptr) return;
	if (AnimInstance->GetCounterReady())
	{
		if (!IPCS->TrySpendMP(3.f)) return;
		OnChangeCounterReadyDelegate->Broadcast(false);
		if (IPCS != nullptr)
		{
			IPCS->SetCounterHitCheck(true);
			GetWorld()->GetTimerManager().SetTimer(CounterTimerHandle, FTimerDelegate::CreateLambda([&] {
				IPCS->SetCounterHitCheck(false);
				UE_LOG(LogTemp, Log, TEXT("Counter Handle Timer Exceeded"));
				}), 0.25f, false
			);
		}
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK2], 0.15f, 0.05f, 1.f);
		LastPlayedAnimSequence = PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK2;
		LMB_ComboCount = 2;
	}
	else if (!AnimInstance->GetBusy())
	{
		if (!IPCS->TrySpendMP(3.f)) return;
		CorrectCharacterRotation(true);
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK1 + LMB_ComboCount], 0.05f, 0.05f);
		LastPlayedAnimSequence = PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK1 + LMB_ComboCount;
		LMB_ComboCount += 1;
		LMB_ComboCount %= 3;
	}
}

void ADOCCharacter::RMB()
{
	if (!EquippedActors.Contains(0)) return;
	if (IPCUI != nullptr && IPCUI->IsInventoryVisible()) return;
	if (bSwaySucceedBonus)
	{
		if (!IPCS->TrySpendMP(3.f)) return;
		CorrectCharacterRotation(true);
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_QTE_ATTACK], 0.05f, 0.05f);
		LastPlayedAnimSequence = PLAYER_ANIMATION_SEQUENCE_QTE_ATTACK;
	}
	else if (AnimInstance != nullptr && !AnimInstance->GetBusy())
	{
		if (!IPCS->TrySpendMP(5.f)) return;
		CorrectCharacterRotation(true);
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK1 + RMB_ComboCount], 0.05f, 0.05f);
		LastPlayedAnimSequence = PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK1 + RMB_ComboCount;
		RMB_ComboCount += 1;
		RMB_ComboCount %= 2;
	}
}

void ADOCCharacter::Roll()
{
	if (IPCUI != nullptr && IPCUI->IsInventoryVisible()) return;
	if (AnimInstance != nullptr && !AnimInstance->GetBusy())
	{
		if (MovementVector.Y == 0.f && MovementVector.X == 0.f)
		{
			if (!IPCS->TrySpendMP(3.5f)) return;
			AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_DASH], 0.25f, 0.25f, 1.f, 0.2f);
			LastPlayedAnimSequence = PLAYER_ANIMATION_SEQUENCE_DASH;
			FTimerManager& TimerManager = GetWorld()->GetTimerManager();
			TimerManager.ClearTimer(SwayTimerHandle);
			CurrentSwayLaunchCount = TotalSwayLaunchCount;
			TimerManager.SetTimer(SwayTimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					if (CurrentSwayLaunchCount <= 0.f)
					{
						TimerManager.ClearTimer(SwayTimerHandle);
					}
					LaunchCharacter(-GetActorForwardVector() * 100.f * CurrentSwayLaunchCount, true, false);
					CurrentSwayLaunchCount--;
				}
			), SwayRate, true);
			SetInvincibleMoment(0.22f, true);
		}
		else
		{
			if (!IPCS->TrySpendMP(7.f)) return;
			CorrectCharacterRotation(false);
			AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_ROLL], 0.25f, 1.f);
			LastPlayedAnimSequence = PLAYER_ANIMATION_SEQUENCE_ROLL;
			SetInvincibleMoment(1.65f, false);
		}
	}
}

void ADOCCharacter::Quickslot(const FInputActionValue& Value)
{
	if (IPCUI != nullptr && IPCUI->IsInventoryVisible()) return;
	const float ScalarValue = Value.Get<float>();
	const int32 SlotIndex = FMath::TruncToInt32(ScalarValue);
	OnQuickSlotInputDelegate.Broadcast(SlotIndex - 1);
}

void ADOCCharacter::ShiftTriggered()
{
	if (IPCUI != nullptr && IPCUI->IsInventoryVisible()) return;
	if (AnimInstance != nullptr && !AnimInstance->GetBusy())
	{
		if (!IPCS->TrySpendMP(10.f)) return;
		OnChangeCounterReadyDelegate->Broadcast(true);
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_READY], 0.25f, 0.25f);
	}
}

void ADOCCharacter::ShiftCompleted()
{
	if (IPCUI != nullptr && IPCUI->IsInventoryVisible()) return;
	if (AnimInstance != nullptr && AnimInstance->GetCounterReady())
	{
		OnChangeCounterReadyDelegate->Broadcast(false);
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_RELEASE], 0.75f, 0.75f);
	}
}

void ADOCCharacter::FStarted()
{
	if (!EquippedActors.Contains(0)) return;	// Weapon Equip Test
	if (LockedOnMonster != nullptr) ToExecuteMonster = Cast<IIDamagable>(LockedOnMonster);	// Locked On Target
	else if (InteractableItem != nullptr) ToExecuteMonster = Cast<IIDamagable>(InteractableItem);	// Pointing Target
	if (ToExecuteMonster == nullptr || !ToExecuteMonster->IsExecutable()) return;
	if (AnimInstance != nullptr && !AnimInstance->GetBusy())
	{
		FDamageConfig ExecuteDamageConfig;
		ExecuteDamageConfig.AttackType = ATTACK_TYPE_COUNTER;
		ExecuteDamageConfig.bIsCrit = true;
		ExecuteDamageConfig.Instigator = GetController();
		ExecuteDamageConfig.Causer = this;
		ToExecuteMonster->Execute(ExecuteDamageConfig);
		FVector MonsterLocation = Cast<AActor>(ToExecuteMonster)->GetActorLocation();
		FVector MonsterDirection = (GetActorLocation() - MonsterLocation).GetSafeNormal2D();
		SetActorLocation(GetActorLocation() + MonsterDirection * 50.f, true);
		SetActorRotation((-MonsterDirection).Rotation());
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_EXECUTE]);
		SetInvincibleMoment(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_EXECUTE]->GetPlayLength() + 0.2f, false);
	}
}

bool ADOCCharacter::RecieveDamage(FDamageConfig DamageConfig)
{
	if (bSway)
	{
		bSwaySucceedBonus = true;
		SetSkeletalMeshesCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER_DODGE_CHARGED);
		GetWorld()->GetTimerManager().SetTimer(SwaySucceedBonusTimerHandle, FTimerDelegate::CreateLambda([&]() { 
			bSwaySucceedBonus = false; 
			SetSkeletalMeshesCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
			}), 0.5f, false);
		return false;
	}
	if (bInvincible) return false;

	OnReceivedDamage.Broadcast(DamageConfig);
	OnChangeCounterReadyDelegate->Broadcast(false);
	if (AnimInstance != nullptr) AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_FLINCH]);
	if (IPCS != nullptr) IPCS->SetCounterHitCheck(false);
	
	LaunchCharacter(DamageConfig.HitDirection * DamageConfig.Damage * 500.f, true, false);
	return true;
}

void ADOCCharacter::LockOnMonster(IIEnemyCharacter* Enemy)
{
	if (LockedOnMonster != nullptr)
	{
		if (OnDeathLockFreeDelegateHandle.IsValid()) LockedOnMonster->GetOnDeathDelegate()->Remove(OnDeathLockFreeDelegateHandle);
	}

	LockedOnMonster = Enemy;
	if (LockedOnParticleSystemComponent != nullptr && PlayerGazeComponent != nullptr)
	{
		LockedOnParticleSystemComponent->SetWorldRotation((Enemy->GetLocation() - GetActorLocation()).GetSafeNormal2D().Rotation());
		LockedOnParticleSystemComponent->SetWorldLocation((Enemy->GetLocation() + GetActorLocation()) / 2.f * FVector(1.f, 1.f, 0.f) + FVector(0.f, 0.f, Enemy->GetLocation().Z));
		LockedOnParticleSystemComponent->SetRelativeScale3D(FVector(1.f * FMath::Min(FVector::Dist(Enemy->GetLocation(), GetActorLocation()), 500.f) / 500.f));
		LockedOnParticleSystemComponent->Activate();
		LockedOnParticleSystemComponent->SetVisibility(true);

		OnDeathLockFreeDelegateHandle = Enemy->GetOnDeathDelegate()->AddLambda([this, Enemy](FDamageConfig DamageConfig)
		{
			if (LockedOnMonster == Enemy)
			{
				LockFreeMonster();
			}
		});

		PlayerGazeComponent->SetLockedOnTarget(Enemy);
		SetToPerspectiveCamera(FollowCamera->GetComponentTransform());
		bUseControllerRotationYaw = true;
	}
}

void ADOCCharacter::LockFreeMonster()
{
	if (LockedOnParticleSystemComponent != nullptr && PlayerGazeComponent != nullptr)
	{
		LockedOnParticleSystemComponent->SetVisibility(false);
		LockedOnParticleSystemComponent->Deactivate();
		SetToFollowCamera();
		IIInteractableItem* Interactable = Cast<IIInteractableItem>(LockedOnMonster);
		if (Interactable != nullptr) Interactable->UnSelect();
		PlayerGazeComponent->SetLockedOnTarget(nullptr);
		bUseControllerRotationYaw = false;

		if (LockedOnMonster != nullptr)
		{
			if (OnDeathLockFreeDelegateHandle.IsValid()) LockedOnMonster->GetOnDeathDelegate()->Remove(OnDeathLockFreeDelegateHandle);
		}
	}
	LockedOnMonster = nullptr;
}

FTransform ADOCCharacter::GetCameraTransform()
{
	if (FollowCamera->IsActive()) return GetFollowCamera() != nullptr ? GetFollowCamera()->GetComponentTransform() : FTransform::Identity;
	return PerspectiveCamera != nullptr ? PerspectiveCamera->GetComponentTransform() : FTransform::Identity;
}

void ADOCCharacter::AdjustRootBone(FVector AdjustVector, bool bLaunch, bool bAllowReverse)
{
	DynamicCameraLocation += AdjustVector * 0.5f;
	//GetCameraBoom()->SetRelativeLocation(GetCameraBoom()->GetRelativeLocation() + AdjustVector * 0.5f);
	//GetMesh()->SetRelativeLocation(GetMesh()->GetRelativeLocation() + AdjustVector);
	if (!bAllowReverse && AdjustVector.X > 0.f) return;
	if (bLaunch && AdjustVector.Size() > 0.f)
	{
		FRotator ControllRotation = GetActorRotation();
		FVector LaunchDirection = -ControllRotation.RotateVector(AdjustVector);
		LaunchDirection.X *= 20.f;
		LaunchDirection.Y *= 20.f;
		LaunchDirection.Z = FMath::Max(0.f, LaunchDirection.Z);
		LaunchCharacter(LaunchDirection, true, false);
	}
}

void ADOCCharacter::AdjustMeshRotation(FRotator AdjustRotator)
{
	GetMesh()->SetRelativeRotation(GetMesh()->GetRelativeRotation() - AdjustRotator);
}

void ADOCCharacter::AdjustMesh(FVector VerticalVector, FRotator AdjustRotator, FVector LaunchVector)
{
	GetMesh()->SetRelativeRotation(GetMesh()->GetRelativeRotation() - AdjustRotator);
	GetMesh()->SetRelativeLocation(GetMesh()->GetRelativeLocation() - VerticalVector);
	if (LaunchVector.X > 0.f) return;
	FVector LaunchDirection = -GetActorRotation().RotateVector(LaunchVector);
	LaunchDirection.X *= LastPlayedAnimSequence == PLAYER_ANIMATION_SEQUENCE_ROLL ? 100.f : 20.f;
	LaunchDirection.Y *= LastPlayedAnimSequence == PLAYER_ANIMATION_SEQUENCE_ROLL ? 100.f : 20.f;
	LaunchDirection.Z = FMath::Max(0.f, LaunchDirection.Z);
	LaunchCharacter(LaunchDirection, true, false);
}

bool ADOCCharacter::AttachEquipment(AActor* ToAttachActor, int32 Type, FName SocketName)
{
	if (EquippedActors.Contains(Type) && EquippedActors[Type] != nullptr)
	{
		DetachEquipment(Type);
	}
	IIEquipment* Equipment = Cast<IIEquipment>(ToAttachActor);
	if (Equipment != nullptr) EquippedActors.Add(Type, Equipment);
	bool bAttached = ToAttachActor->GetRootComponent()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	return bAttached;
}

bool ADOCCharacter::AttachEquipment(IIEquipment* ToEquip, int32 Type)
{
	if (ToEquip == nullptr) return false;

	USkeletalMesh* ToEquipMesh = ToEquip->GetSkeletalMesh();
	if (ToEquipMesh == nullptr) return false;

	switch (Type)
	{
	case EQUIPMENT_HELMET:
		HelmetMesh->SetSkeletalMesh(ToEquipMesh);
		EquippedActors.Add(EQUIPMENT_HELMET, ToEquip);
		break;
	case EQUIPMENT_TORSO1:
	case EQUIPMENT_TORSO2:
	case EQUIPMENT_TORSO3:
		TorsoMesh->SetSkeletalMesh(ToEquipMesh);
		EquippedActors.Add(EQUIPMENT_TORSO1, ToEquip);
		break;
	case EQUIPMENT_GLOVE:
		GauntletsMesh->SetSkeletalMesh(ToEquipMesh);
		EquippedActors.Add(EQUIPMENT_GLOVE, ToEquip);
		break;
	case EQUIPMENT_PANTS:
		LegsMesh->SetSkeletalMesh(ToEquipMesh);
		EquippedActors.Add(EQUIPMENT_PANTS, ToEquip);
		break;
	case EQUIPMENT_SHOSE:
		BootsMesh->SetSkeletalMesh(ToEquipMesh);
		EquippedActors.Add(EQUIPMENT_SHOSE, ToEquip);
		break;
	default:
		return false;
	}

	OnEquipmentChanged.ExecuteIfBound(Type, ToEquipMesh);
	//OnEquipmentChanged.Broadcast(Type, ToEquipMesh);
	return true;
}

IIEquipment* ADOCCharacter::DetachEquipment(int32 ItemCode)
{
	if (ItemCode == EQUIPMENT_SWORD)
	{
		IIEquipment** FoundActor = EquippedActors.Find(ItemCode);
		if (FoundActor != nullptr && *FoundActor != nullptr)
		{
			AActor* Actor = Cast<AActor>(*FoundActor);
			if(Actor)
				Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			
			EquippedActors.Remove(ItemCode);
			return *FoundActor;
		}
	}
	else
	{
		USkeletalMesh* NullMesh = nullptr;
		switch (ItemCode)
		{
			case EQUIPMENT_HELMET: HelmetMesh->SetSkeletalMesh(NullMesh); break;
			case EQUIPMENT_TORSO1:
			case EQUIPMENT_TORSO2:
			case EQUIPMENT_TORSO3:
				TorsoMesh->SetSkeletalMesh(NullMesh); break;
			case EQUIPMENT_GLOVE: GauntletsMesh->SetSkeletalMesh(NullMesh); break;
			case EQUIPMENT_PANTS: LegsMesh->SetSkeletalMesh(NullMesh); break;
			case EQUIPMENT_SHOSE: BootsMesh->SetSkeletalMesh(NullMesh); break;
			default: break;
		}

		OnEquipmentChanged.ExecuteIfBound(ItemCode, NullMesh);
		if (EquippedActors.Contains(ItemCode))
		{
			IIEquipment* OriginalActor = EquippedActors.FindAndRemoveChecked(ItemCode);
			return OriginalActor;
		}
	}

	return nullptr;
}

void ADOCCharacter::ResetTraceProperties()
{
	if (HitBoxComponent != nullptr) HitBoxComponent->ResetProperties();
}

void ADOCCharacter::PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, float DamageAmount)
{
	if (HitBoxComponent != nullptr)
	{
		FVector SwingDirection;
		TArray<FHitResult> temp = HitBoxComponent->PerformCapsuleTrace<UIDamagable>(CapsuleRadius, CapsuleHalfHeight, Location, Rotation, Precision, SwingDirection);

		for (FHitResult HitResult : temp)
		{
			if (HitResult.GetActor() != nullptr)
			{
				IIDamagable* Damagable = Cast<IIDamagable>(HitResult.GetActor());
				if (Damagable != nullptr)
				{
					FDamageConfig DamageConfig;
					DamageConfig.Damage = DamageAmount;
					DamageConfig.HitDirection = SwingDirection;
					DamageConfig.HitLocation = HitResult.ImpactPoint;
					DamageConfig.HitParticleType = PARTICLE_PLAYER_HIT_MELLEE_IMPACT;
					DamageConfig.AttackType = ATTACK_TYPE_MELLE;
					DealDamage(Damagable, DamageConfig);
				}
			}
		}
	}
}

bool ADOCCharacter::PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, FDamageConfig DamageConfig)
{
	bool rtn = false;
	if (HitBoxComponent != nullptr)
	{
		FVector SwingDirection;
		TArray<FHitResult> temp = HitBoxComponent->PerformCapsuleTrace<UIDamagable>(CapsuleRadius, CapsuleHalfHeight, Location, Rotation, Precision, SwingDirection);

		for (FHitResult HitResult : temp)
		{
			if (HitResult.GetActor() != nullptr)
			{
				IIDamagable* Damagable = Cast<IIDamagable>(HitResult.GetActor());
				if (Damagable != nullptr)
				{
					DamageConfig.HitDirection = SwingDirection;
					DamageConfig.HitLocation = HitResult.ImpactPoint;
					DamageConfig.HitParticleType = PARTICLE_PLAYER_HIT_MELLEE_IMPACT;
					DamageConfig.AttackType = ATTACK_TYPE_MELLE;
					if (!rtn) rtn = DealDamage(Damagable, DamageConfig);
				}
			}
		}
	}
	return rtn;
}

bool ADOCCharacter::DealDamage(IIDamagable* Damagable, FDamageConfig& DamageConfig)
{
	FPlayerStat CurrStat;
	if (IPCS != nullptr) CurrStat = IPCS->GetPlayerStat();

	DamageConfig.bIsCrit = FMath::FRandRange(0.f, 100.f) < CurrStat.CriticalRate ? true : false;
	DamageConfig.Causer = this;
	DamageConfig.Instigator = GetController();
	DamageConfig.Damage = DamageConfig.Damage * (1.f + CurrStat.AttackPower / 100.f);
	if (DamageConfig.bIsCrit) DamageConfig.Damage *= 1.5f;
	DamageConfig.CausedTimeSeconds = GetWorld()->TimeSeconds;

	if (Damagable != nullptr)
	{
		if (Damagable->RecieveDamage(DamageConfig) && IPCS != nullptr)
		{
			IPCS->DealtDamage(DamageConfig);
			return true;
		}
		//DrawDebugSphere(GetWorld(), DamageConfig.HitLocation, 20.f, 12, FColor::Red, false, 2.f);
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Damage Dealt: %f"), DamageConfig.Damage));
	}
	return false;
}

void ADOCCharacter::Execute(FDamageConfig DamageConfig)
{
	if (ToExecuteMonster == nullptr) return;
	DamageConfig.AttackType = ATTACK_TYPE_COUNTER;
	DamageConfig.bIsCrit = true;
	//DamageConfig.Instigator = GetController();
	DamageConfig.Causer = Cast<AActor>(ToExecuteMonster);
	//DamageConfig.Damage = 3.f;
	DamageConfig.HitLocation = GetMesh()->GetSocketLocation("Weapon_Tip_R");
	DamageConfig.HitDirection = GetActorUpVector();
	DamageConfig.HitParticleType = PARTICLE_PLAYER_HIT_MELLEE_IMPACT;
	//DealDamage(ToExecuteMonster, DamageConfig);
	CounterAttackSucceeded(DamageConfig);
}

void ADOCCharacter::Catch(float Duration, float PlayRate, FDamageConfig DamageConfig)
{
	if (LockedOnMonster != nullptr) ToExecuteMonster = Cast<IIDamagable>(LockedOnMonster);	// Locked On Target
	else if (InteractableItem != nullptr) ToExecuteMonster = Cast<IIDamagable>(InteractableItem);	// Pointing Target
	if (ToExecuteMonster == nullptr) return;

	DamageConfig.AttackType = ATTACK_TYPE_COUNTER;
	DamageConfig.bIsCrit = true;
	DamageConfig.Instigator = GetController();
	DamageConfig.Causer = this;
	ToExecuteMonster->Execute(DamageConfig);
	FVector MonsterLocation = Cast<AActor>(ToExecuteMonster)->GetActorLocation();
	FVector MonsterDirection = (GetActorLocation() - MonsterLocation).GetSafeNormal2D();
	SetActorLocation(MonsterLocation + MonsterDirection * 10.f, true);
	SetActorRotation((-MonsterDirection).Rotation());
	ToExecuteMonster->Stun(Duration + 1.f, DamageConfig);

	AnimInstance->SetCurrentMontagePlayRate(PlayRate);
	GetWorld()->GetTimerManager().ClearTimer(CounterTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(CounterTimerHandle, FTimerDelegate::CreateLambda([&, DamageConfig]() {
		AnimInstance->SetCurrentMontagePlayRate(1.f);
		Execute(DamageConfig);
		}), Duration, false
	);

}

void ADOCCharacter::CounterAttackSucceeded(FDamageConfig DamageConfig)
{
    if (DamageConfig.Causer != nullptr)
    {
		FDamageConfig DealingDamageConfig;
		if (DamageConfig.AttackType != ATTACK_TYPE_COUNTER)
		{
			FVector CauserLocation = DamageConfig.Causer->GetActorLocation();
			FVector DirectVector = (DamageConfig.Causer->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			FVector TargetLocation = CauserLocation + DirectVector * 50.0f;

			FVector CurrentLocation = GetActorLocation();

			TargetLocation.Z = CurrentLocation.Z;

			SetActorLocation(TargetLocation, false);
			LaunchCharacter(DirectVector * 150.f, true, false);
		}
		else
		{
			DealingDamageConfig.Groggy = DamageConfig.Groggy;
		}
		GetWorld()->GetTimerManager().ClearTimer(CounterTimerHandle);

		FPlayerStat CurrStat;
		if (IPCS != nullptr) CurrStat = IPCS->GetPlayerStat();

		StoredCounterDamagable = DamageConfig.Causer;
		DealingDamageConfig.Damage = DamageConfig.Damage;
		DealingDamageConfig.HitDirection = (DamageConfig.HitDirection * -1.f).GetSafeNormal();
		DealingDamageConfig.AttackType = ATTACK_TYPE_COUNTER;
		DealingDamageConfig.HitParticleType = PARTICLE_PLAYER_HIT_MELLEE_IMPACT;
		DealingDamageConfig.HitLocation = DamageConfig.Causer->GetActorLocation();

		TotalCounterDamageCount = 5 + FMath::FloorToInt32(CurrStat.AttackPower / 100.f);
		CurrentCounterDamageCount = 0;
		GetWorld()->GetTimerManager().SetTimer(CounterTimerHandle, FTimerDelegate::CreateLambda([this, WeakDamagable = TWeakObjectPtr<AActor>(Cast<AActor>(DamageConfig.Causer)), CapturedDealingDamageConfig = DealingDamageConfig]() mutable {
			if (CurrentCounterDamageCount >= TotalCounterDamageCount)
			{
				GetWorld()->GetTimerManager().ClearTimer(CounterTimerHandle);
			}
			else
			{
				if (WeakDamagable.IsValid())
				{
					IIDamagable* CurrentDamagable = Cast<IIDamagable>(WeakDamagable.Get());
					if (CurrentDamagable != nullptr)
					{
						CapturedDealingDamageConfig.HitLocation += CapturedDealingDamageConfig.HitDirection * CurrentCounterDamageCount * 3.f;
						DealDamage(CurrentDamagable, CapturedDealingDamageConfig);
					}
				}
				CurrentCounterDamageCount++;
			}
			}), 0.05f, true
		);
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADOCCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ADOCCharacter::Roll);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADOCCharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADOCCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ADOCCharacter::MoveEnd);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADOCCharacter::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADOCCharacter::Interact);
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ADOCCharacter::ToggleInventory);
		EnhancedInputComponent->BindAction(WidemapAction, ETriggerEvent::Started, this, &ADOCCharacter::TurnOnWidemap);
		EnhancedInputComponent->BindAction(WidemapAction, ETriggerEvent::Completed, this, &ADOCCharacter::TurnOffWidemap);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Started, this, &ADOCCharacter::LMB);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Started, this, &ADOCCharacter::RMB);
		EnhancedInputComponent->BindAction(QuickslotAction, ETriggerEvent::Started, this, &ADOCCharacter::Quickslot);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ADOCCharacter::ShiftTriggered);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ADOCCharacter::ShiftCompleted);
		EnhancedInputComponent->BindAction(FAction, ETriggerEvent::Started, this, &ADOCCharacter::FStarted);
	}

}

void ADOCCharacter::Move(const FInputActionValue& Value)
{
	MovementVector = Value.Get<FVector2D>();
	if (IPCUI != nullptr && IPCUI->IsInventoryVisible()) return;
	if (PerspectiveCamera->IsActive() && LockedOnMonster == nullptr) return;
	if (AnimInstance == nullptr || AnimInstance->GetBusy()) return;
	// input is a Vector2D

	if (Controller != nullptr)
	{
		if (GetCharacterMovement() != nullptr)
		{
			if (MovementVector.Y <= 0.f)
			{
				GetCharacterMovement()->MaxWalkSpeed = 300.f;
			}
			else if (FMath::Abs(MovementVector.X) > 0.f)
			{
				GetCharacterMovement()->MaxWalkSpeed = 400.f;
			}
			else
			{
				GetCharacterMovement()->MaxWalkSpeed = 600.f;
			}
		}

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	CorrectCharacterRotation(true);
}

void ADOCCharacter::MoveEnd(const FInputActionValue& Value)
{
	MovementVector.X = 0.f;
	MovementVector.Y = 0.f;
}

void ADOCCharacter::Look(const FInputActionValue& Value)
{
	if (PerspectiveCamera->IsActive()) return;
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADOCCharacter::CorrectCharacterRotation(bool bForcedForward)
{
	if (Controller != nullptr && !MovementVector.IsNearlyZero())
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		FVector WorldDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
		WorldDirection.Normalize();

		FRotator NewRotation = WorldDirection.Rotation();
		SetActorRotation(bForcedForward ? YawRotation : NewRotation);
	}
}


void ADOCCharacter::SetSkeletalMeshesCustomDepthStencilValue(int32 StencilValue)
{
	for (USkeletalMeshComponent* SkeletalMeshComponent : SkeletalMeshComponents)
	{
		if (SkeletalMeshComponent)
		{
			SkeletalMeshComponent->SetRenderCustomDepth(true);
			SkeletalMeshComponent->SetCustomDepthStencilValue(StencilValue);
		}
	}
}




