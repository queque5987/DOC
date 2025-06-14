#pragma once

#include "CoreMinimal.h"
#include "CStageStructs.generated.h"

USTRUCT(BlueprintType)
struct FDamageConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Instigator = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Causer = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitDirection = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitLocation = FVector::ZeroVector;
};