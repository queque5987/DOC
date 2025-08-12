#include "Player/UI/CMonsterHP.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/CStageDelegateTypes.h"
#include "Player/UI/CMonsterHPWidget.h"

UCMonsterHP::UCMonsterHP() : Super()
{
	SetWidgetSpace(EWidgetSpace::World);
	SetDrawSize(FVector2D(140.0f, 20.0f));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bDrawAtDesiredSize = true;
	SetTwoSided(false);
	SetReceivesDecals(false);
	SetCastShadow(false);
	SetOnlyOwnerSee(false);

	ConstructorHelpers::FClassFinder<UUserWidget> HPWidgetClassFinder(TEXT("/Game/UI/BP_MonsterHP"));
	if (HPWidgetClassFinder.Succeeded()) SetWidgetClass(HPWidgetClassFinder.Class);

	CurrentInterpSpeed = BaseInterpSpeed;
	TimeAboveThreshold = 0.0f;

	//HPChangedDelegate = nullptr;
	MonsterHPWidgetInstance = nullptr;
}

void UCMonsterHP::SetDelegates(FOnStatusChanged* InStatusChangedDelegate, FOnGroggy* InGroggyDelegate, FOnGroggyEnd* InGroggyEndDelegate)
{
	StatusChangedDelegate = InStatusChangedDelegate;

	MonsterHPWidgetInstance = Cast<UCMonsterHPWidget>(GetUserWidgetObject());

	if (MonsterHPWidgetInstance)
	{
		if (StatusChangedDelegate != nullptr)
		{
			StatusChangedDelegate->AddUFunction(this, TEXT("UpdateHP"));
			//HPChangedDelegate->BindUFunction(this, TEXT("UpdateHP"));
		}
		if (InGroggyDelegate != nullptr)
		{
			InGroggyDelegate->AddLambda([&](FDamageConfig DamageConfig) {
				if (MonsterHPWidgetInstance != nullptr) MonsterHPWidgetInstance->SetGroggyPanelVisibility(true);
				}
			);
		}
		if (InGroggyEndDelegate != nullptr)
		{
			InGroggyEndDelegate->AddLambda([&]() {
				if (MonsterHPWidgetInstance != nullptr) MonsterHPWidgetInstance->SetGroggyPanelVisibility(false);
				}
			);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCMonsterHP: Failed to cast UserWidgetObject to UCMonsterHPWidget."));
	}
}

void UCMonsterHP::UpdateHP(FPlayerStat MonsterStat)
{
	float NewHP = MonsterStat.CurrHP;
	float MaxHP = MonsterStat.MaxHP;

	SetVisibility(NewHP > 0.0f);

	if (MonsterHPWidgetInstance)
	{
		MonsterHPWidgetInstance->UpdateBar(MonsterStat);

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(HPDelayTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(
				HPDelayTimerHandle,
				this,
				&UCMonsterHP::OnHPDelayTimerExpired,
				1.0f,
				false
			);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCMonsterHP: MonsterHPWidgetInstance is null in UpdateHP."));
	}
}

void UCMonsterHP::OnHPDelayTimerExpired()
{
	if (MonsterHPWidgetInstance)
	{
		MonsterHPWidgetInstance->StartDelayInterpolation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCMonsterHP: MonsterHPWidgetInstance is null in OnHPDelayTimerExpired."));
	}
}

void UCMonsterHP::SetTransform(FVector NewLocation, FRotator NewRotation, float DeltaSeconds)
{
	const FVector CurrentLocation = GetComponentLocation();
	const FRotator CurrentRotation = GetComponentRotation();

	const float Distance = FVector::Dist(CurrentLocation, NewLocation);
	const float Angle = FMath::Abs((CurrentRotation - NewRotation).GetNormalized().Yaw);

	if (Distance > DistanceThreshold || Angle > AngleThreshold)
	{
		TimeAboveThreshold += DeltaSeconds;
		CurrentInterpSpeed = FMath::Min(BaseInterpSpeed + TimeAboveThreshold * AccelerationRate, MaxInterpSpeed);
	}
	else
	{
		TimeAboveThreshold = 0.0f;
		CurrentInterpSpeed = BaseInterpSpeed;
	}

	SetWorldLocationAndRotationNoPhysics(
		FMath::VInterpTo(CurrentLocation, NewLocation, DeltaSeconds, CurrentInterpSpeed),
		FMath::RInterpTo(CurrentRotation, NewRotation, DeltaSeconds, CurrentInterpSpeed)
	);
}