#include "DOCCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IStageBuild.h"
#include "IPlayerControllerStage.h"
#include "IPlayerControllerUI.h"
#include "IInteractableItem.h"
#include "GameFramework/GameModeBase.h"
#include "DrawDebugHelpers.h"

//////////////////////////////////////////////////////////////////////////
// ADOCCharacter

ADOCCharacter::ADOCCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 0.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ConstructorHelpers::FObjectFinder<UInputAction> InteractFinder(TEXT("/Game/ThirdPerson/Input/Actions/IA_Interact.IA_Interact"));
	if (InteractFinder.Succeeded()) InteractAction = InteractFinder.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> InventoryFinder(TEXT("/Game/ThirdPerson/Input/Actions/IA_Inventory.IA_Inventory"));
	if (InventoryFinder.Succeeded()) InventoryAction = InventoryFinder.Object;
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
}

void ADOCCharacter::StopJumping()
{
	Super::StopJumping();
}

void ADOCCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	FVector TraceStartLocation = GetFollowCamera()->GetComponentLocation();
	FVector CamFor = GetFollowCamera()->GetForwardVector();
	CamFor.Z = 0.f;
	FVector TraceEndLocation = TraceStartLocation + CamFor.GetSafeNormal() * 4000.f;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceEndLocation, ECollisionChannel::ECC_Pawn, CollisionQueryParams);
	DrawDebugSphere(GetWorld(), HitResult.bBlockingHit ? HitResult.Location : TraceEndLocation, 50.f, 32, HitResult.bBlockingHit ? FColor::Green : FColor::Red);
	if (IPCS != nullptr) IPCS->SightReached(HitResult.bBlockingHit ? HitResult.Location : TraceEndLocation);

	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceStartLocation + GetFollowCamera()->GetForwardVector() * 500.f, ECollisionChannel::ECC_GameTraceChannel1, CollisionQueryParams);
	IIInteractableItem* tempItem = Cast<IIInteractableItem>(HitResult.GetActor());

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
}

IIPlayerControllerStage* ADOCCharacter::GetPlayerControllerStage()
{
	return GetController() != nullptr ? Cast<IIPlayerControllerStage>(GetController()) : nullptr;
}

UObject* ADOCCharacter::GetControllerAsObject()
{
	return GetController() != nullptr ? GetController() : nullptr;
}

void ADOCCharacter::Interact()
{
	if (InteractableItem != nullptr) InteractableItem->Interact(IPCUI, IPCS);
	InteractableItem = nullptr;
}

void ADOCCharacter::ToggleInventory()
{
	if (IPCUI != nullptr) IPCUI->ToggleInventory();
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
	}

}

void ADOCCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
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
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




