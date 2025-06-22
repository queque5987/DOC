#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPlayerGazeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOC_API UCPlayerGazeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPlayerGazeComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PerspectiveCamera;

	class IIPlayerOnStage* PlayerCharacter;
	class IIEnemyCharacter* LockedOnMonster;
	class IIObjectPoolManager* ObjectPoolManager;

protected:
	TSet<class UStaticMeshComponent*> PrevCamBlockingStaticMeshes;
	TSet<class UStaticMeshComponent*> CamBlockingStaticMeshes;

	virtual void BeginPlay() override;
	void SetStaticMeshTranslucent(class UStaticMeshComponent* StaticMeshComponent, float Opacity);
	void SetStaticMeshOpaque(class UStaticMeshComponent* StaticMeshComponent);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void InitializeProperties(class IIPlayerOnStage* Character, class UCameraComponent* FollowCam, class UCameraComponent* PerspectiveCam, class IIObjectPoolManager* GameState) {
		PlayerCharacter = Character;
		FollowCamera = FollowCam;
		PerspectiveCamera = PerspectiveCam;
		ObjectPoolManager = GameState;
	};
	void SetLockedOnTarget(class IIEnemyCharacter* TargetMonster);
};
