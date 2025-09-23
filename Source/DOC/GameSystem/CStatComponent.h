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

	FOnStatusChanged OnStatusChanged;
	FOnDeath OnDeath;
	FOnGroggy OnGroggy;

	FOnReceivedDamage* OnReceivedDamageDelegate;
	FOnGroggyEnd* OnGroggyEndDelegate;

	void SetupDelegates(FOnReceivedDamage* InOnReceivedDamageDelegate, FOnGroggyEnd* InOnGroggyEndDelegate);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void TakeDamage(FDamageConfig DamageConfig);

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetCurrentHP() const { return Stat.CurrHP; }

	void SetCurrentHP(float NewHP);

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxHP() const { return Stat.MaxHP; }

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetMaxHP(float NewMaxHP);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	bool IsGroggy() { return bGroggy; }

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetMaxGroggy(float NewMaxGroggy) { Stat.MaxGroggy = NewMaxGroggy; }

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetGroggy(float NewGroggy) { Stat.Groggy = NewGroggy; }

	FPlayerStat* GetPlayerStat() { return &Stat; }

	int32 GetExecutableCount() { return ExecutableCount; };
	void SetExecutableCount(int32 InExecutableCount) { ExecutableCount = InExecutableCount; };
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	FPlayerStat Stat;

	bool bGroggy;
	int32 ExecutableCount = 0;
	FTimerHandle LastHitTimerHandle;
		
};
