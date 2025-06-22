#include "Player/CPlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACPlayerCameraManager::ACPlayerCameraManager() : Super()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeLocation(FVector(0.f, 20.f, 50.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PerspectiveCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PerspectiveCamera"));
	PerspectiveCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PerspectiveCamera->bUsePawnControlRotation = false;
	PerspectiveCamera->SetActive(false);
	PerspectiveCamera->SetAutoActivate(false);
}

void ACPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	PerspectiveCamera->SetActive(false);
}

void ACPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	FMinimalViewInfo CamView;
	if (PerspectiveCamera != nullptr && PerspectiveCamera->IsActive())
	{
		PerspectiveCamera->GetCameraView(DeltaTime, CamView);
	}
	else
	{
		FollowCamera->GetCameraView(DeltaTime, CamView);
	}
	SetCameraCachePOV(CamView);
}

void ACPlayerCameraManager::ActivateFollowCamera()
{
	if (PerspectiveCamera != nullptr && FollowCamera != nullptr)
	{
		PerspectiveCamera->SetActive(false);
		FollowCamera->SetActive(true);
	}
}

void ACPlayerCameraManager::ActivatePerspectiveCamera(FTransform Transform)
{
	if (PerspectiveCamera != nullptr && FollowCamera != nullptr)
	{
		PerspectiveCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		PerspectiveCamera->SetWorldTransform(Transform);
		FollowCamera->SetActive(false);
		PerspectiveCamera->SetActive(true);
	}
}

FTransform ACPlayerCameraManager::GetFollowCameraTransform()
{
	return FollowCamera != nullptr ? FollowCamera->GetComponentTransform() : FTransform();
}

FTransform ACPlayerCameraManager::GetPerspectiveCameraTransform()
{
	return PerspectiveCamera != nullptr ? PerspectiveCamera->GetComponentTransform() : FTransform();
}

void ACPlayerCameraManager::SetPerspectiveCameraTransform(FVector Location, FRotator Rotation, FVector Scale3D, bool bWorldPosition)
{
	if (bWorldPosition) PerspectiveCamera->SetWorldLocationAndRotation(Location, Rotation);
	else PerspectiveCamera->SetRelativeLocationAndRotation(Location, Rotation);
}

FVector ACPlayerCameraManager::GetFollowCameraForwardVector()
{
	return FollowCamera != nullptr ? FollowCamera->GetForwardVector() : FVector();
}

FVector ACPlayerCameraManager::GetPerspectiveCameraRightVector()
{
	return PerspectiveCamera != nullptr ? PerspectiveCamera->GetRightVector() : FVector();
}

FVector ACPlayerCameraManager::GetPerspectiveCameraUpVector()
{
	return PerspectiveCamera != nullptr ? PerspectiveCamera->GetUpVector() : FVector();
}

bool ACPlayerCameraManager::GetIsPerspective()
{
	return PerspectiveCamera->IsActive();
}
