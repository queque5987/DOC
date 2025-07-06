#include "Player/UI/CMonsterHPWidget.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

void UCMonsterHPWidget::UpdateHPBar(float CurrentHP, float MaxHP)
{
	TargetHealthPercent = CurrentHP / MaxHP;

	if (HPBar)
	{
		HPBar->SetPercent(TargetHealthPercent);
	}

	// 새로운 데미지가 들어오면 지연 보간 중지
	StopDelayInterpolation();

	// HPBar_Delay는 NativeTick에서 점진적으로 업데이트되므로 여기서는 초기화만
	// (선택 사항: 데미지 직후 HPBar_Delay를 HPBar와 동일하게 만들고 싶다면 아래 주석 해제)
	// if (HPBar_Delay)
	// {
	// 	CurrentDelayHealthPercent = TargetHealthPercent;
	// 	HPBar_Delay->SetPercent(CurrentDelayHealthPercent);
	// }
}

void UCMonsterHPWidget::SetDelayHP(float NewDelayHPPercent)
{
	// 이 함수는 HPBar_Delay를 즉시 업데이트할 때 사용됩니다.
	if (HPBar_Delay)
	{
		HPBar_Delay->SetPercent(NewDelayHPPercent);
		CurrentDelayHealthPercent = NewDelayHPPercent; // 현재 지연 HP도 업데이트
	}
	// 이 함수가 호출되면 지연 보간은 중지됩니다.
	StopDelayInterpolation();
}

void UCMonsterHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TargetHealthPercent = 1.0f;
	CurrentDelayHealthPercent = 1.0f;
	bIsDelaying = false; // 초기에는 지연 보간 비활성화

	// 위젯이 생성될 때 HPBar와 HPBar_Delay를 초기화
	if (HPBar) HPBar->SetPercent(1.0f);
	if (HPBar_Delay) HPBar_Delay->SetPercent(1.0f);
}

void UCMonsterHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsDelaying && HPBar_Delay)
	{
		// HPBar_Delay를 TargetHealthPercent로 부드럽게 보간
		CurrentDelayHealthPercent = FMath::FInterpTo(CurrentDelayHealthPercent, TargetHealthPercent, InDeltaTime, DelayInterpSpeed);
		HPBar_Delay->SetPercent(CurrentDelayHealthPercent);

		// 목표에 거의 도달하면 보간 중지
		if (FMath::IsNearlyEqual(CurrentDelayHealthPercent, TargetHealthPercent, 0.001f))
		{
			StopDelayInterpolation();
		}
	}
}

void UCMonsterHPWidget::StartDelayInterpolation()
{
	bIsDelaying = true;
	// 지연 보간 시작 시 현재 HPBar_Delay의 위치를 시작점으로 설정
	if (HPBar_Delay)
	{
		CurrentDelayHealthPercent = HPBar_Delay->GetPercent();
	}
}

void UCMonsterHPWidget::StopDelayInterpolation()
{
	bIsDelaying = false;
}
