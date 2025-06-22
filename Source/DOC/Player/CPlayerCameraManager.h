// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class DOC_API ACPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	ACPlayerCameraManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PerspectiveCamera;
	
public:
	virtual void BeginPlay() override;
	virtual void UpdateCamera(float DeltaTime) override;
	void ActivateFollowCamera();
	void ActivatePerspectiveCamera(FTransform Transform);
	FTransform GetFollowCameraTransform();
	FTransform GetPerspectiveCameraTransform();
	void SetPerspectiveCameraTransform(FVector Location, FRotator Rotation, FVector Scale3D = FVector::ZeroVector, bool bWorldPosition = true);
	FVector GetFollowCameraForwardVector();
	FVector GetPerspectiveCameraRightVector();
	FVector GetPerspectiveCameraUpVector();
	bool GetIsPerspective();
};
