#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/CStageStructs.h"
#include "CProjectile.generated.h"

UCLASS()
class DOC_API ACProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ACProjectile();

protected:
	virtual void BeginPlay() override;
	class USceneComponent* SceneComponent;
	class UParticleSystemComponent* ParticleSystemComponent;
	class USphereComponent* Collider;
	class IIObjectPoolManager* ObjectPoolManager;
	struct FDamageConfig Config;
	FCollisionQueryParams CollisionQueryParam;
	FVector Dir;
	float Vel;
	float MaxRange;
	float Trail;
public:	
	void Fire(struct FDamageConfig DamageConfig, FVector Direction, float Velocity, float Range);
	void SetParticleSystemComponent(class UParticleSystemComponent* PSC);
	void SetObjectPoolManager(class IIObjectPoolManager* OPM);
	virtual void Tick(float DeltaTime) override;
};
