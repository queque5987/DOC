#pragma once

#include "CoreMinimal.h"
#include "CStageStructs.generated.h"

USTRUCT(BlueprintType)
struct FDamageConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Damage = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Groggy = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    bool bIsCrit = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    AActor* Instigator = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    AActor* Causer = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class USoundBase* HitSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UParticleSystem* HitEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile_Effects")
    class UParticleSystem* ProjectileSpawnedEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile_Effects")
    class UParticleSystem* ProjectileTrailEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile_Effects")
    class UParticleSystem* ProjectileExpiredEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile_Effects")
    class UParticleSystem* Projectile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    int32 HitParticleType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FLinearColor DamageWidgetColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
    FVector HitDirection = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
    FVector HitLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit_React")
    FVector KnockBack = FVector(0.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit_React")
    float KnockBackTime = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
    bool KnockDown = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    float CausedTimeSeconds = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
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

USTRUCT(BlueprintType)
struct FRelative2DLocation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    float DistFromTop = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    float DistFromBottom = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    float DistFromLeft = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    float DistFromRight = 0.f;
};

USTRUCT(BlueprintType)
struct FPlayerTimeSeriesData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TArray<float> TimeStamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TArray<int32> PlayerButtonSeries;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TArray<float> RelativeDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    TArray<float> DistFromTop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    TArray<float> DistFromBottom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    TArray<float> DistFromLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    TArray<float> DistFromRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TArray<float> PlayerHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TArray<float> PlayerStamina;
};
