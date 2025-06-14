#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DrawDebugHelpers.h"
#include "CHitBoxComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOC_API UCHitBoxComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCHitBoxComponent();

protected:
	virtual void BeginPlay() override;
	bool bInit;
	bool bDebug;
	FVector PrevLocation;
	FRotator PrevRotation;
	FCollisionQueryParams CQP;
	TSet<AActor*> AleardyHitActors;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ResetProperties() {
		bInit = true; 
		CQP.ClearIgnoredActors();
		CQP.AddIgnoredActor(GetOwner());
		AleardyHitActors.Empty();
	};

	template<typename T>
	TArray<FHitResult> PerformCapsuleTrace(float CapsuleRadius, float CapsuleHalfHeight, FVector Location, FRotator Rotation, int32 Precision, FVector& SwingDirection)
	{
		TArray<FHitResult> rtn;
		FCollisionShape tempShape;
		tempShape.SetCapsule(CapsuleRadius, CapsuleHalfHeight);

		if (bInit || Precision <= 1)
		{
			TArray<FHitResult> tempResults;
			GetWorld()->SweepMultiByObjectType(tempResults,
				Location,
				Location,
				Rotation.Quaternion(),
				FCollisionObjectQueryParams::AllObjects,
				tempShape,
				CQP

			);
			bool flagdebug = false;
			for (FHitResult HR : tempResults)
			{
				if (HR.GetActor() != nullptr && HR.GetActor()->Implements<T>())
				{
					if (!AleardyHitActors.Contains(HR.GetActor()))
					{
						CQP.AddIgnoredActor(HR.GetActor());
						rtn.Add(HR);
						flagdebug = true;
						AleardyHitActors.Add(HR.GetActor());
					}
				}
			}
			if (bDebug)
			{
				DrawDebugCapsule(GetWorld(),
					Location,
					CapsuleHalfHeight,
					CapsuleRadius,
					Rotation.Quaternion(),
					(flagdebug ? FColor::Green : FColor::Red),
					false,
					1.f,
					0U,
					0.5f
				);
			}
			bInit = false;
			SwingDirection = FVector::ZeroVector;
		}
		else
		{
			for (int32 p = 0; p < Precision - 1; p++)
			{
				TArray<FHitResult> tempResults;
				GetWorld()->SweepMultiByObjectType(tempResults,
					FMath::Lerp(PrevLocation, Location, (float)p / (float)Precision),
					FMath::Lerp(PrevLocation, Location, (float)(p + 1) / (float)Precision),
					FMath::Lerp(PrevRotation, Rotation, (float)p / (float)Precision).Quaternion(),
					FCollisionObjectQueryParams::AllObjects,
					tempShape,
					CQP
					
				);
				bool flagdebug = false;
				for (FHitResult HR : tempResults)
				{
					if (HR.GetActor() != nullptr && HR.GetActor()->Implements<T>())
					{
						if (!AleardyHitActors.Contains(HR.GetActor()))
						{
							CQP.AddIgnoredActor(HR.GetActor());
							rtn.Add(HR);
							flagdebug = true;
							AleardyHitActors.Add(HR.GetActor());
						}
					}
				}
				if (bDebug)
				{
					DrawDebugCapsule(GetWorld(),
						FMath::Lerp(PrevLocation, Location, (float)p / (float)Precision),
						CapsuleHalfHeight,
						CapsuleRadius,
						FMath::Lerp(PrevRotation, Rotation, (float)p / (float)Precision).Quaternion(),
						(flagdebug ? FColor::Green : FColor::Red),
						false,
						1.f,
						0U,
						0.5f
					);
				}
				SwingDirection = (Location - PrevLocation).GetSafeNormal();
			}
		}
		PrevLocation = Location;
		PrevRotation = Rotation;
		
		return rtn;
	};
};
