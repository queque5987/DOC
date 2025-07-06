#include "GameSystem/CStatComponent.h"
#include "TimerManager.h" // FTimerManager를 사용하기 위해 추가

UCStatComponent::UCStatComponent()
{
	MaxHP = 100.0f;
	CurrentHP = MaxHP;
	PrimaryComponentTick.bCanEverTick = false;
}


void UCStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	OnHPChanged.ExecuteIfBound(CurrentHP, MaxHP);
}

void UCStatComponent::TakeDamage(float InDamage)
{
	if (InDamage <= 0.0f) return;

	const float PrevHP = CurrentHP;
	CurrentHP = FMath::Clamp(CurrentHP - InDamage, 0.0f, MaxHP);

	UE_LOG(LogTemp, Warning, TEXT("%s took %.2f damage. HP: %.2f -> %.2f"),
		*GetOwner()->GetName(), InDamage, PrevHP, CurrentHP);

	OnHPChanged.ExecuteIfBound(CurrentHP, MaxHP);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(LastHitTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			LastHitTimerHandle,
			[&]()
			{
				OnHPDelayUpdateInit.ExecuteIfBound();
			},
			1.0f,
			false
		);
	}
}

void UCStatComponent::SetMaxHP(float NewMaxHP)
{
	if (NewMaxHP <= 0.0f) return;

	MaxHP = NewMaxHP;
	CurrentHP = FMath::Min(CurrentHP, MaxHP);

	OnHPChanged.ExecuteIfBound(CurrentHP, MaxHP);
}

