#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "CStatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOC_API UCStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCStatComponent();

	FHP_CHANGED OnHPChanged;
	FHP_DELAY_UPDATE_INIT OnHPDelayUpdateInit;

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void TakeDamage(float InDamage);

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetCurrentHP() const { return CurrentHP; }

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxHP() const { return MaxHP; }

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetMaxHP(float NewMaxHP);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	UPROPERTY(VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentHP;

	FTimerHandle LastHitTimerHandle;
		
};
