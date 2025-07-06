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
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/CPlayerGazeComponent.h"
#include "Interfaces/IAnimInstance.h"
#include "GameSystem/CHitBoxComponent.h"
#include "DrawDebugHelpers.h"

ADOCCharacter::ADOCCharacter()
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKMeshFinder(TEXT("/Game/QuangPhan/G2_Mercenaries/Meshes/Characters/Combines/SK_LP287_MercA.SK_LP287_MercA"));

	if (SKMeshFinder.Succeeded()) GetMesh()->SetSkeletalMesh(SKMeshFinder.Object);
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -97.f),
		FRotator(0.f, 0.f, 270.f)
	);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
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

	ConstructorHelpers::FObjectFinder<UAnimSequence> RMB_ATTACK1_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_01.anim_attack_light_01"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> RMB_ATTACK2_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_02.anim_attack_light_02"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> RMB_ATTACK3_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_med_02.anim_attack_med_02"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> LMB_ATTACK1_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_03.anim_attack_light_03"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> LMB_ATTACK2_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_light_04.anim_attack_light_04"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> LMB_ATTACK3_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_med_01.anim_attack_med_01"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> COUNTER_READY_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_heavy_startup.anim_attack_heavy_startup"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> COUNTER_IDLE_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_heavy_idle.anim_attack_heavy_idle"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> COUNTER_ATTACK_Finder(TEXT("/Game/Sword_Animation/Animations/anim_attack_heavy_release.anim_attack_heavy_release"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> EXECUTE_Finder(TEXT("/Game/Sword_Animation/Animations/anim_execute.anim_execute"));

	AnimSeqArr.SetNum(PLAYER_ANIMATION_SEQUENCE_NUM);
	if (LMB_ATTACK1_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK1]		= (LMB_ATTACK1_Finder.Object);
	if (LMB_ATTACK2_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK2]		= (LMB_ATTACK2_Finder.Object);
	if (LMB_ATTACK3_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK3]		= (LMB_ATTACK3_Finder.Object);
	if (RMB_ATTACK1_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK1]		= (RMB_ATTACK1_Finder.Object);
	if (RMB_ATTACK2_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK2]		= (RMB_ATTACK2_Finder.Object);
	if (RMB_ATTACK3_Finder.Succeeded())		AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK3]		= (RMB_ATTACK3_Finder.Object);
	if (COUNTER_READY_Finder.Succeeded())	AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_READY]		= (COUNTER_READY_Finder.Object);
	if (COUNTER_IDLE_Finder.Succeeded())	AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_IDLE]		= (COUNTER_IDLE_Finder.Object);
	if (COUNTER_ATTACK_Finder.Succeeded())	AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_COUNTER_ATTACK]	= (COUNTER_ATTACK_Finder.Object);
	if (EXECUTE_Finder.Succeeded())			AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_EXECUTE]			= (EXECUTE_Finder.Object);

	HitBoxComponent = CreateDefaultSubobject<UCHitBoxComponent>(TEXT("HitBoxComponent"));
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
	IPCUI = Cast<IIPlayerControllerUI>(GetController());
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOMDEPTH_PLAYERCHARACTER);
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
	}
	//if (HitBoxComponent != nullptr)HitBoxComponent->SetDebug(true);
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
	//FVector CamBoomLoc = GetCameraBoom()->GetComponentLocation() - GetCameraBoom()->GetForwardVector()* GetCameraBoom()->TargetArmLength;
	//FVector FollowCamLoc = GetFollowCamera()->GetComponentLocation();
	//GetFollowCamera()->SetWorldLocation(FMath::Lerp(FollowCamLoc, CamBoomLoc, DeltaSeconds));
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
	if (AnimInstance != nullptr && !AnimInstance->GetBusy())
	{
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_LMB_ATTACK1 + LMB_ComboCount]);
		LMB_ComboCount += 1;
		LMB_ComboCount %= 3;
	}
}

void ADOCCharacter::RMB()
{
	if (AnimInstance != nullptr && !AnimInstance->GetBusy())
	{
		AnimInstance->PlayAnimation(AnimSeqArr[PLAYER_ANIMATION_SEQUENCE_RMB_ATTACK1 + RMB_ComboCount]);
		RMB_ComboCount += 1;
		RMB_ComboCount %= 2;
	}
}

bool ADOCCharacter::RecieveDamage(FDamageConfig DamageConfig)
{
	IPCS->RecieveDamage(DamageConfig);
	LaunchCharacter(DamageConfig.HitDirection * DamageConfig.Damage * 500.f, true, false);
	//DrawDebugDirectionalArrow(GetWorld(), DamageConfig.HitLocation - DamageConfig.Damage * 500.f, DamageConfig.HitLocation, 100.f, FColor::Red, false, 1.f, 0U, 1.f);
	return false;
}

void ADOCCharacter::LockOnMonster(IIEnemyCharacter* Enemy)
{
	LockedOnMonster = Enemy;
	if (LockedOnParticleSystemComponent != nullptr && PlayerGazeComponent != nullptr)
	{
		LockedOnParticleSystemComponent->SetWorldRotation((Enemy->GetLocation() - GetActorLocation()).GetSafeNormal2D().Rotation());
		LockedOnParticleSystemComponent->SetWorldLocation((Enemy->GetLocation() + GetActorLocation()) / 2.f * FVector(1.f, 1.f, 0.f) + FVector(0.f, 0.f, Enemy->GetLocation().Z));
		LockedOnParticleSystemComponent->SetRelativeScale3D(FVector(1.f * FMath::Min(FVector::Dist(Enemy->GetLocation(), GetActorLocation()), 500.f) / 500.f));
		LockedOnParticleSystemComponent->Activate();
		LockedOnParticleSystemComponent->SetVisibility(true);

		PlayerGazeComponent->SetLockedOnTarget(Enemy);
		SetToPerspectiveCamera(FollowCamera->GetComponentTransform());
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
	}
	LockedOnMonster = nullptr;
}

FTransform ADOCCharacter::GetCameraTransform()
{
	return GetFollowCamera() != nullptr ? GetFollowCamera()->GetComponentTransform() : FTransform::Identity;
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
	LaunchDirection.X *= 20.f;
	LaunchDirection.Y *= 20.f;
	LaunchDirection.Z = FMath::Max(0.f, LaunchDirection.Z);
	LaunchCharacter(LaunchDirection, true, false);
}

bool ADOCCharacter::AttachEquipment(AActor* ToAttachActor, int32 Type, FName SocketName)
{
	return ToAttachActor->GetRootComponent()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	//return ToAttachActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
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
					DamageConfig.Causer = this;
					DamageConfig.Instigator = GetController();
					DamageConfig.Damage = DamageAmount;
					DamageConfig.HitDirection = SwingDirection;
					DamageConfig.HitLocation = HitResult.ImpactPoint;
					DamageConfig.HitParticleType = PARTICLE_PLAYER_HIT_MELLEE_IMPACT;
					Damagable->RecieveDamage(DamageConfig);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADOCCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADOCCharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADOCCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADOCCharacter::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADOCCharacter::Interact);
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &ADOCCharacter::ToggleInventory);
		EnhancedInputComponent->BindAction(WidemapAction, ETriggerEvent::Started, this, &ADOCCharacter::TurnOnWidemap);
		EnhancedInputComponent->BindAction(WidemapAction, ETriggerEvent::Completed, this, &ADOCCharacter::TurnOffWidemap);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Started, this, &ADOCCharacter::LMB);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Started, this, &ADOCCharacter::RMB);
	}

}

void ADOCCharacter::Move(const FInputActionValue& Value)
{
	if (PerspectiveCamera->IsActive() && LockedOnMonster == nullptr) return;
	// input is a Vector2D
	MovementVector = Value.Get<FVector2D>();

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




