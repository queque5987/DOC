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
    float Groggy = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Instigator = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Causer = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitDirection = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HitParticleType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsCrit = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor DamageWidgetColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CausedTimeSeconds = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 AttackType = 0;
};

USTRUCT(BlueprintType)
struct FPlayerStat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CurrHP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxMP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CurrMP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Groggy = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxGroggy = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackPower = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefencePower = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealthRegenPower = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CriticalRate = 0.f;
};