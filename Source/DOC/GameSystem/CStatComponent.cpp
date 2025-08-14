#include "GameSystem/CStatComponent.h"
#include "TimerManager.h"

UCStatComponent::UCStatComponent()
{
	Stat.MaxHP = 100.0f;
	Stat.MaxMP = 100.f;
	Stat.CurrHP = Stat.MaxHP;
	Stat.CurrMP = Stat.MaxMP;
	PrimaryComponentTick.bCanEverTick = false;
}

void UCStatComponent::BeginPlay()
{
	Super::BeginPlay();
	OnStatusChanged.Broadcast(Stat);
}

void UCStatComponent::TakeDamage(FDamageConfig DamageConfig)
{
	if (DamageConfig.Damage <= 0.0f) return;
	bool IsAlreadyGroggy = Stat.Groggy >= Stat.MaxGroggy;
	const float PrevHP = Stat.CurrHP;
	Stat.CurrHP = FMath::Clamp(Stat.CurrHP - DamageConfig.Damage, 0.f, Stat.MaxHP);
	Stat.Groggy = FMath::Clamp(Stat.Groggy + DamageConfig.Groggy, 0.f, Stat.MaxGroggy);
	OnStatusChanged.Broadcast(Stat);

	if (Stat.CurrHP <= 0.f)
	{
		OnDeath.Broadcast(DamageConfig);
	}
	else if (!IsAlreadyGroggy && Stat.Groggy >= Stat.MaxGroggy)
	{
		OnGroggy.Broadcast(DamageConfig);
		bGroggy = true;
	}
}

void UCStatComponent::SetCurrentHP(float NewHP)
{
	Stat.CurrHP = NewHP;
	if (Stat.MaxHP < NewHP) Stat.MaxHP = NewHP;

	OnStatusChanged.Broadcast(Stat);
}

void UCStatComponent::SetMaxHP(float NewMaxHP)
{
	if (NewMaxHP <= 0.0f) return;
	Stat.MaxHP = NewMaxHP;
	Stat.CurrHP = FMath::Min(Stat.CurrHP, Stat.MaxHP);

	OnStatusChanged.Broadcast(Stat);
}

void UCStatComponent::SetupDelegates(FOnReceivedDamage* InOnReceivedDamageDelegate, FOnGroggyEnd* InOnGroggyEndDelegate)
{
	OnReceivedDamageDelegate = InOnReceivedDamageDelegate;
	if (OnReceivedDamageDelegate != nullptr)
	{
		OnReceivedDamageDelegate->AddUFunction(this, TEXT("TakeDamage"));
	}
	OnGroggyEndDelegate = InOnGroggyEndDelegate;
	if (OnGroggyEndDelegate != nullptr)
	{
		OnGroggyEndDelegate->AddLambda([&]() {
			Stat.Groggy = 0.f;
			bGroggy = false;
			OnStatusChanged.Broadcast(Stat);
			}
		);
	}
}
