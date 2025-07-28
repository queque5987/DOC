#include "Player/UI/CDamage.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Player/UI/CDamageWidget.h"
#include "Interfaces/IPlayerControllerStage.h"
#include "CDamage.h"

UCDamage::UCDamage()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> DamageWidgetBPClass(TEXT("/Game/UI/BP_Damage")); // Replace with your actual blueprint path
	if (DamageWidgetBPClass.Class != nullptr)
	{
		DamageWidgetClass = DamageWidgetBPClass.Class;
		SetWidgetClass(DamageWidgetClass);
		SetDrawSize(FVector2D(100.0f, 50.0f));
		SetWidgetSpace(EWidgetSpace::Screen);
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
}

void UCDamage::SetDamageConfig(FDamageConfig DamageConfig)
{
	if (DamageWidgetClass != nullptr)
	{
		DamageWidgetInstance = Cast<UCDamageWidget>(GetUserWidgetObject());
		if (DamageWidgetInstance != nullptr)
		{
			DamageWidgetInstance->SetWidget(DamageConfig);
		}
		HitDirection = DamageConfig.HitDirection;
		FloatingSpeed = DamageConfig.Damage * 10.f;
		SpawnStamp = DamageConfig.CausedTimeSeconds;
	}
}

void UCDamage::SetController(IIPlayerControllerStage* InPlayerControllerStage)
{
	PlayerControllerStage = InPlayerControllerStage;
}

void UCDamage::SetWorldLocationAndActivate(FVector InLocation)
{
	SetWorldLocation(InLocation);
	Activate();
}

void UCDamage::Activate(bool bReset)
{
	Super::Activate(bReset);
	if (DamageWidgetInstance != nullptr)
	{
		DamageWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		DamageWidgetInstance->SetRenderOpacity(1.f);
		Opacity = 1.f;
	}
}

void UCDamage::Deactivate()
{
	Super::Deactivate();
	if (DamageWidgetInstance != nullptr) DamageWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
}

void UCDamage::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetWorldLocation(GetComponentLocation() - HitDirection * 10.f * DeltaTime);
	FloatingSpeed *= 0.3f;
	Opacity -= DeltaTime;
	UpdateRotationToCamera();
	DamageWidgetInstance->SetRenderOpacity(FMath::Min(Opacity * 2.f, 1.f));
}

void UCDamage::UpdateRotationToCamera()
{
	if (PlayerControllerStage != nullptr)
	{
		FRotator CameraRotation = PlayerControllerStage->GetCurrentCameraRotation();
		SetWorldRotation(CameraRotation);
	}
}
