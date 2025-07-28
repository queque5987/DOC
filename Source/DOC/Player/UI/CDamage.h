#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/CStageStructs.h"
#include "CDamage.generated.h"

UCLASS()
class DOC_API UCDamage : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UCDamage();

	void SetDamageConfig(FDamageConfig DamageConfig);
	void SetController(class IIPlayerControllerStage* InPlayerControllerStage);
	void SetWorldLocationAndActivate(FVector InLocation);
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	float SpawnStamp;
private:
	UPROPERTY()
	class UCDamageWidget* DamageWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DamageWidgetClass;

	class IIPlayerControllerStage* PlayerControllerStage;

	void UpdateRotationToCamera();

	FVector HitDirection;
	float FloatingSpeed;
	float Opacity;
};
