#include "Player/CPlayerGazeComponent.h"
#include "Camera/CameraComponent.h"
#include "Interfaces/IEnemyCharacter.h"
#include "Interfaces/IPlayerOnStage.h"
#include "Interfaces/IObjectPoolManager.h"
#include "Components/StaticMeshComponent.h"

UCPlayerGazeComponent::UCPlayerGazeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCPlayerGazeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCPlayerGazeComponent::SetStaticMeshTranslucent(UStaticMeshComponent* StaticMeshComponent, float Opacity)
{
	if (StaticMeshComponent != nullptr && ObjectPoolManager != nullptr)
	{
		TArray<UMaterialInterface*> MIArray;
		ObjectPoolManager->GetTranslucentMaterialInstance(StaticMeshComponent->GetStaticMesh(), MIArray);
		if (MIArray.Num() > 0)
		{
			for (int32 i = 0; i < MIArray.Num(); i++)
			{
				if (MIArray[i] != nullptr) StaticMeshComponent->SetMaterial(i, MIArray[i]);
			}
			StaticMeshComponent->bForceDisableNanite = true;
		}
	}
}

void UCPlayerGazeComponent::SetStaticMeshOpaque(UStaticMeshComponent* StaticMeshComponent)
{
	if (StaticMeshComponent != nullptr && ObjectPoolManager != nullptr)
	{
		TArray<UMaterialInterface*> MIArray;
		ObjectPoolManager->GetOpaqueMaterialInstance(StaticMeshComponent->GetStaticMesh(), MIArray);
		if (MIArray.Num() > 0)
		{
			for (int32 i = 0; i < MIArray.Num(); i++)
			{
				if (MIArray[i] != nullptr) StaticMeshComponent->SetMaterial(i, MIArray[i]);
			}
			StaticMeshComponent->bForceDisableNanite = false;
		}
	}
}

void UCPlayerGazeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if ((LockedOnMonster != nullptr && PlayerCharacter != nullptr) && 
		ObjectPoolManager != nullptr)
	{
		FVector LockLoation = LockedOnMonster->GetLocation();
		FVector CurrLocation = PlayerCharacter->GetLocation();
		FVector UpLocation = CurrLocation + PlayerCharacter->GetUpVector() * 100.f;
		FVector CurrentCamLocation = PerspectiveCamera->GetComponentLocation();

		FVector ExpectedCamDirection = (UpLocation - LockLoation).GetSafeNormal();
		FVector CurrentCamDirection = (UpLocation - CurrentCamLocation).GetSafeNormal();

		FVector UpBackLocation = UpLocation + ExpectedCamDirection * 200.f;
		float CamToGoDist = FVector::Dist(CurrentCamLocation, UpBackLocation);
		float ExpectedDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CurrentCamDirection, UpBackLocation)));
		float PerTickRotateDeg = 30.f * DeltaTime;;
		float ToRotateAlpha = FMath::Min(PerTickRotateDeg / ExpectedDeg, 1.f);

		//LockedOnParticleSystemComponent->SetWorldRotation((LockLoation - CurrLocation).GetSafeNormal2D().Rotation());
		//LockedOnParticleSystemComponent->SetWorldLocation((LockLoation + CurrLocation) / 2.f * FVector(1.f, 1.f, 0.f) + FVector(0.f, 0.f, LockLoation.Z));
		//LockedOnParticleSystemComponent->SetRelativeScale3D(FVector(1.f * FMath::Min(FVector::Dist(LockLoation, CurrLocation), 500.f) / 500.f));

		//PerspectiveCamera->SetWorldLocation(FMath::Lerp(CurrentCamLocation, UpBackLocation, ToRotateAlpha));
		PerspectiveCamera->SetWorldLocation(UpBackLocation);
		PerspectiveCamera->SetWorldRotation((-ExpectedCamDirection).Rotation());
		PlayerCharacter->Controller_SetControlRotation((-ExpectedCamDirection * FVector(1.f, 1.f, 0.f)).Rotation());
		TArray<FHitResult> tempResults;

		FVector CamRV = PerspectiveCamera->GetRightVector();
		FVector CamUV = PerspectiveCamera->GetUpVector();

		FVector UpLeft = CurrentCamLocation - CamRV * 200.f + CamUV * 150.f;
		FVector UpRight = CurrentCamLocation + CamRV * 200.f + CamUV * 150.f;

		FCollisionQueryParams tempCQP;
		FCollisionShape CollisionShape;
		float SphereSize = 75.f;
		CollisionShape.SetSphere(SphereSize);
		for (int32 d = 0; d < 3; d++)
		{
			for (int32 e = 0; e < 3; e++)
			{
				FVector TraceLocation = CurrentCamLocation + FMath::Lerp(CamRV, -CamRV, d / 2.f) * 200.f + FMath::Lerp(CamUV, FVector::ZeroVector, e / 2.f) * 150.f;
				FVector TracePlayerLocation = CurrLocation + (TraceLocation - CurrLocation).GetSafeNormal() * SphereSize * 2.f;
				GetWorld()->SweepMultiByObjectType(tempResults, TraceLocation, TracePlayerLocation, FQuat::Identity, FCollisionObjectQueryParams::AllStaticObjects, CollisionShape, tempCQP);
				for (const FHitResult& tempResult : tempResults)
				{
					if (tempResult.GetComponent() != nullptr)
					{
						CamBlockingStaticMeshes.Add(Cast<UStaticMeshComponent>(tempResult.GetComponent()));
						tempCQP.AddIgnoredComponent(tempResult.GetComponent());
						UE_LOG(LogTemp, Log, TEXT("Tracing %s"), *tempResult.GetComponent()->GetName());
					}
				}
			}
		}

		for (UStaticMeshComponent* SM : CamBlockingStaticMeshes)
		{
			if (!PrevCamBlockingStaticMeshes.Contains(SM))
			{
				SetStaticMeshTranslucent(SM, 0.2f);
			}
		}
		for (UStaticMeshComponent* SM : PrevCamBlockingStaticMeshes)
		{
			if (!CamBlockingStaticMeshes.Contains(SM))
			{
				SM->bForceDisableNanite = false;
				SetStaticMeshOpaque(SM);
			}
		}
		PrevCamBlockingStaticMeshes = CamBlockingStaticMeshes;
		CamBlockingStaticMeshes.Empty();
	}
}

void UCPlayerGazeComponent::SetLockedOnTarget(IIEnemyCharacter* TargetMonster)
{
	LockedOnMonster = TargetMonster;
}

