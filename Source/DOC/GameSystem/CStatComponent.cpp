#include "GameSystem/CStatComponent.h"
#include "TimerManager.h" // FTimerManager를 사용하기 위해 추가

UCStatComponent::UCStatComponent()
{
	Stat.MaxHP = 100.0f;
	Stat.CurrHP = Stat.MaxHP;
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

	const float PrevHP = Stat.CurrHP;
	Stat.CurrHP = FMath::Clamp(Stat.CurrHP - DamageConfig.Damage, 0.0f, Stat.MaxHP);

	UE_LOG(LogTemp, Warning, TEXT("%s took %.2f damage. HP: %.2f -> %.2f"),
		*GetOwner()->GetName(), DamageConfig.Damage, PrevHP, Stat.CurrHP);

	OnStatusChanged.Broadcast(Stat);

	if (Stat.CurrHP <= 0.0f)
	{
		OnDeath.Broadcast(DamageConfig);
	}
}

void UCStatComponent::SetMaxHP(float NewMaxHP)
{
	if (NewMaxHP <= 0.0f) return;
	Stat.MaxHP = NewMaxHP;
	Stat.CurrHP = FMath::Min(Stat.CurrHP, Stat.MaxHP);

	OnStatusChanged.Broadcast(Stat);
}

