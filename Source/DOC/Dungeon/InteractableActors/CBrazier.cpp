#include "CBrazier.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PointLightComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

ACBrazier::ACBrazier()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SMC_Brazier_Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Brazier_Body"));
	SMC_Brazier_Top = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Brazier_Top"));
	NC_Brazier_FX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent_Brazier_FX"));
	PLC_Brazier_PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent_Brazier_PointLight"));
	
	SetRootComponent(SceneComponent);
	SMC_Brazier_Body->SetupAttachment(SceneComponent);
	SMC_Brazier_Top->SetupAttachment(SMC_Brazier_Body);
	NC_Brazier_FX->SetupAttachment(SMC_Brazier_Top);
	PLC_Brazier_PointLight->SetupAttachment(SMC_Brazier_Top);

	NC_Brazier_FX->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	PLC_Brazier_PointLight->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	PLC_Brazier_PointLight->SetLightColor(FLinearColor(255.f, 211.f, 170.f));
	PLC_Brazier_PointLight->SetIntensity(0.f);
	PLC_Brazier_PointLight->SetIntensityUnits(ELightUnits::Candelas);
	PLC_Brazier_PointLight->SetAttenuationRadius(500.f);
	PLC_Brazier_PointLight->SetSourceRadius(30.f);
	PLC_Brazier_PointLight->SetUseTemperature(true);
	PLC_Brazier_PointLight->SetTemperature(4500.f);
	PLC_Brazier_PointLight->SetIndirectLightingIntensity(4.f);

	ConstructorHelpers::FObjectFinder<UStaticMesh> Brazier_BodyFinder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Brazier-01-01.SM-Brazier-01-01"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Brazier_TopFinder(TEXT("/Game/Dungeon/Meshes/Prop/SM-Brazier-01-02.SM-Brazier-01-02"));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> Brazier_FXFinder(TEXT("/Game/Dungeon/FX/FX-BrazierEmbers-01.FX-BrazierEmbers-01"));
	
	if (Brazier_BodyFinder.Succeeded() && SMC_Brazier_Body != nullptr) SMC_Brazier_Body->SetStaticMesh(Brazier_BodyFinder.Object);
	if (Brazier_TopFinder.Succeeded() && SMC_Brazier_Top != nullptr) SMC_Brazier_Top->SetStaticMesh(Brazier_TopFinder.Object);
	if (Brazier_FXFinder.Succeeded() && NC_Brazier_FX != nullptr) NC_Brazier_FX->SetAsset(Brazier_FXFinder.Object);

	NC_Brazier_FX->SetAutoActivate(false);
	NC_Brazier_FX->Deactivate();
	PLC_Brazier_PointLight->SetAutoActivate(false);
	PLC_Brazier_PointLight->Deactivate();
	PLC_Brazier_PointLight->SetVisibility(false);
}

void ACBrazier::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void ACBrazier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PLIntensity += MaxPLIntensity * DeltaTime;
	if (PLIntensity >= MaxPLIntensity)
	{
		PLIntensity = MaxPLIntensity;
		SetActorTickEnabled(false);
	}
	PLC_Brazier_PointLight->SetIntensity(PLIntensity);
}

void ACBrazier::Interact(IIPlayerControllerUI* PlayerControllerUI, IIPlayerControllerStage* PlayerControllerStage)
{
	if (TurnOn) return;
	TurnOn = true;
	SetActorTickEnabled(true);
	NC_Brazier_FX->Activate();
	PLC_Brazier_PointLight->Activate();
	PLC_Brazier_PointLight->SetVisibility(true);
}

void ACBrazier::Select()
{
	if (Selected) return;
	Selected = true;
	if (SMC_Brazier_Top != nullptr)
	{
		SMC_Brazier_Top->SetRenderCustomDepth(true);
		SMC_Brazier_Top->SetCustomDepthStencilValue(bBusy ? CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY : CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
	if (SMC_Brazier_Body != nullptr)
	{
		SMC_Brazier_Body->SetRenderCustomDepth(true);
		SMC_Brazier_Body->SetCustomDepthStencilValue(bBusy ? CUSTOMDEPTH_INTERACTABLE_ITEM_BUSY : CUSTOMDEPTH_INTERACTABLE_ITEM);
	}
}

void ACBrazier::UnSelect()
{
	if (!Selected) return;
	Selected = false;
	if (SMC_Brazier_Top != nullptr)
	{
		SMC_Brazier_Top->SetRenderCustomDepth(false);
	}
	if (SMC_Brazier_Body != nullptr)
	{
		SMC_Brazier_Body->SetRenderCustomDepth(false);
	}
}

void ACBrazier::SetVisibility(bool e)
{
	if (e)
	{
		SceneComponent->Activate();
		SMC_Brazier_Top->Activate();
		SMC_Brazier_Body->Activate();
		NC_Brazier_FX->Activate();
		PLC_Brazier_PointLight->Activate();
	}
	else
	{
		SceneComponent->Deactivate();
		SMC_Brazier_Top->Deactivate();
		SMC_Brazier_Body->Deactivate();
		NC_Brazier_FX->Deactivate();
		TurnOn = false;
		PLIntensity = 0.f;
		PLC_Brazier_PointLight->SetIntensity(PLIntensity);
		PLC_Brazier_PointLight->Deactivate();
	}
	SMC_Brazier_Top->SetVisibility(e);
	SMC_Brazier_Body->SetVisibility(e);
	SMC_Brazier_Top->SetHiddenInGame(!e);
	SMC_Brazier_Body->SetHiddenInGame(!e);
	PLC_Brazier_PointLight->SetHiddenInGame(!e);
	NC_Brazier_FX->SetHiddenInGame(!e);
	PLC_Brazier_PointLight->SetVisibility(e);
	NC_Brazier_FX->SetVisibility(e);
	SetActorEnableCollision(e);
	UnSelect();
}

bool ACBrazier::GetVisibility()
{
	return (SMC_Brazier_Top->IsActive() && SMC_Brazier_Body->IsActive() ? true : false);
}

