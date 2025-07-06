#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Light.h"
#include "Camera/CameraComponent.h" // UCameraComponent를 위해 추가
#include "CStatusStage.generated.h"

UCLASS()
class DOC_API ACStatusStage : public AActor
{
	GENERATED_BODY()

public:
	ACStatusStage();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override; // Tick 함수 오버라이드

private:
	float TargetLightIntensity; // 목표 라이트 강도
	float CurrentLightIntensity; // 현재 라이트 강도
	float LightFadeSpeed; // 라이트 페이드 속도
	bool bAnimatingLights; // 라이트 애니메이션 중인지 여부

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status Stage")
	USkeletalMeshComponent* DisplayCharacterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status Stage")
	UCameraComponent* CameraComponent;

	// 이 함수를 호출하여 플레이어 시점을 이 액터의 카메라로 전환합니다.
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ActivateStageCamera(class APlayerController* PlayerController, float BlendTime = 0.0f);

	// 이 함수를 호출하여 플레이어 시점을 원래 플레이어 캐릭터로 되돌립니다.
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void DeactivateStageCamera(class APlayerController* PlayerController, AActor* PlayerCharacter, float DelayTime = 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Stage")
	USkeletalMesh* CharacterSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Stage")
	TArray<ALight*> StageLights;

private:
	FTimerHandle CameraTransitionTimerHandle;
	TWeakObjectPtr<class APlayerController> CachedPlayerController; // 타이머 콜백에서 사용하기 위해 캐시
	TWeakObjectPtr<class AActor> CachedPlayerCharacter; // 타이머 콜백에서 사용하기 위해 캐시

	UFUNCTION()
	void OnCameraTransitionTimerExpired();
};