#include "Player/UI/CStatusStage.h"
#include "GameFramework/PlayerController.h"
#include "Components/LightComponent.h"
#include "TimerManager.h" // FTimerManager를 위해 추가

ACStatusStage::ACStatusStage()
{
	PrimaryActorTick.bCanEverTick = true;

	DisplayCharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DisplayCharacterMesh"));
	SetRootComponent(DisplayCharacterMesh);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetRelativeLocation(FVector(0.0f, 200.0f, 170.0f));
	CameraComponent->SetRelativeRotation(FRotator(350.0f, -90.0f, 0.0f));

	// 파츠 컴포넌트 생성 및 설정
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
	HairMesh->SetupAttachment(DisplayCharacterMesh);
	HairMesh->SetMasterPoseComponent(DisplayCharacterMesh);

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(DisplayCharacterMesh);
	HelmetMesh->SetMasterPoseComponent(DisplayCharacterMesh);

	TorsoMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TorsoMesh"));
	TorsoMesh->SetupAttachment(DisplayCharacterMesh);
	TorsoMesh->SetMasterPoseComponent(DisplayCharacterMesh);

	GauntletsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GauntletsMesh"));
	GauntletsMesh->SetupAttachment(DisplayCharacterMesh);
	GauntletsMesh->SetMasterPoseComponent(DisplayCharacterMesh);

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(DisplayCharacterMesh);
	LegsMesh->SetMasterPoseComponent(DisplayCharacterMesh);

	BootsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BootsMesh"));
	BootsMesh->SetupAttachment(DisplayCharacterMesh);
	BootsMesh->SetMasterPoseComponent(DisplayCharacterMesh);

	TargetLightIntensity = 0.0f;
	CurrentLightIntensity = 0.0f;
	LightFadeSpeed = 4.f;
	bAnimatingLights = false;
}

void ACStatusStage::BeginPlay()
{
	Super::BeginPlay();

	for (ALight* Light : StageLights)
	{
		if (Light && Light->GetLightComponent())
		{
			Light->GetLightComponent()->SetIntensity(0.0f);
		}
	}
}

void ACStatusStage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAnimatingLights)
	{
		CurrentLightIntensity = FMath::FInterpTo(CurrentLightIntensity, TargetLightIntensity, DeltaTime, TargetLightIntensity == 0.f ? LightFadeSpeed * 2.f : LightFadeSpeed);

		for (ALight* Light : StageLights)
		{
			if (Light && Light->GetLightComponent())
			{
				Light->GetLightComponent()->SetIntensity(CurrentLightIntensity);
			}
		}

		if (FMath::IsNearlyEqual(CurrentLightIntensity, TargetLightIntensity, 0.01f))
		{
			bAnimatingLights = false;
		}
	}
}

void ACStatusStage::ActivateStageCamera(APlayerController* PlayerController, float BlendTime)
{
	if (PlayerController)
	{
		// 기존 타이머가 있다면 클리어
		GetWorld()->GetTimerManager().ClearTimer(CameraTransitionTimerHandle);

		PlayerController->SetViewTargetWithBlend(this, BlendTime);

		TargetLightIntensity = 4.5f;
		bAnimatingLights = true;
	}
}

void ACStatusStage::DeactivateStageCamera(APlayerController* PlayerController, AActor* PlayerCharacter, float DelayTime)
{
	if (PlayerController && PlayerCharacter)
	{
		// 타이머를 설정하여 지연 후 플레이어 카메라로 전환
		CachedPlayerController = PlayerController; // 약한 참조로 캐시
		CachedPlayerCharacter = PlayerCharacter; // 약한 참조로 캐시

		GetWorld()->GetTimerManager().SetTimer(
			CameraTransitionTimerHandle,
			this,
			&ACStatusStage::OnCameraTransitionTimerExpired,
			DelayTime, // 지연 시간
			false // 반복 안 함
		);

		// 라이트 페이드 아웃 시작
		TargetLightIntensity = 0.0f;
		bAnimatingLights = true;
	}
}

void ACStatusStage::OnCameraTransitionTimerExpired()
{
	// 타이머 만료 후 플레이어 카메라로 전환
	if (CachedPlayerController.IsValid() && CachedPlayerCharacter.IsValid())
	{
		CachedPlayerController->SetViewTargetWithBlend(CachedPlayerCharacter.Get(), 0.0f); // 블렌딩 없이 즉시 전환
	}
}

void ACStatusStage::SetupDelegates(FOnEquipmentChanged* OnEquipmentChanged)
{
	if (OnEquipmentChanged)
	{
		OnEquipmentChanged->BindUFunction(this, FName("OnPlayerEquipmentChanged"));
	}
}

void ACStatusStage::OnPlayerEquipmentChanged(int32 EquipmentSlot, USkeletalMesh* NewMesh)
{
	switch (EquipmentSlot)
	{
		case EQUIPMENT_HELMET:
			HelmetMesh->SetSkeletalMesh(NewMesh);
			break;
		case EQUIPMENT_TORSO1:
		case EQUIPMENT_TORSO2:
		case EQUIPMENT_TORSO3:
			TorsoMesh->SetSkeletalMesh(NewMesh);
			break;
		case EQUIPMENT_GLOVE:
			GauntletsMesh->SetSkeletalMesh(NewMesh);
			break;
		case EQUIPMENT_PANTS:
			LegsMesh->SetSkeletalMesh(NewMesh);
			break;
		case EQUIPMENT_SHOSE:
			BootsMesh->SetSkeletalMesh(NewMesh);
			break;
		default:
			break;
	}
}