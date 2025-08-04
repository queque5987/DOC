#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Interfaces/CStageStructs.h"
#include "CStatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOC_API UCStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCStatComponent();

	//FHP_CHANGED OnHPChanged;
	FOnStatusChanged OnStatusChanged;
	FOnDeath OnDeath;

	FOnReceivedDamage* OnReceivedDamageDelegate;

	void SetupDelegates(FOnReceivedDamage* InOnReceivedDamageDelegate);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void TakeDamage(FDamageConfig DamageConfig);

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetCurrentHP() const { return Stat.CurrHP; }

	void SetCurrentHP(float NewHP);

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxHP() const { return Stat.MaxHP; }

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetMaxHP(float NewMaxHP);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	FPlayerStat Stat;

	UPROPERTY(EditAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	UPROPERTY(VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float CurrentHP;

	FTimerHandle LastHitTimerHandle;
		
};
