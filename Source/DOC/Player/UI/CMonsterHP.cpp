#include "Player/UI/CMonsterHP.h"
#include "Blueprint/UserWidget.h"

UCMonsterHP::UCMonsterHP() : Super()
{
	// 위젯 공간을 3D 월드로 설정
	SetWidgetSpace(EWidgetSpace::World);

	// 위젯의 크기 설정 (예시: 100x20 픽셀)
	SetDrawSize(FVector2D(100.0f, 20.0f));

	// 위젯 컴포넌트 자체는 충돌을 발생시키지 않도록 설정
	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 위젯이 항상 카메라를 바라보도록 설정 (선택 사항, 필요에 따라 주석 처리)
	bDrawAtDesiredSize = true;
	SetTwoSided(true);
	SetReceivesDecals(false);
	SetCastShadow(false);
	SetOnlyOwnerSee(false);
	SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f)); // 위젯이 플레이어를 바라보도록 초기 회전 설정

	// HPWidgetClass가 유효한 경우, 이 위젯 컴포넌트가 표시할 위젯 클래스를 설정합니다.
	ConstructorHelpers::FClassFinder<UUserWidget> HPWidgetClassFinder(TEXT("/Game/UI/BP_MonsterHP"));
	if (HPWidgetClassFinder.Succeeded())HPWidgetClass = HPWidgetClassFinder.Class;
	if (HPWidgetClass)
	{
		SetWidgetClass(HPWidgetClass);
	}
}

void UCMonsterHP::SetMonsterHP(float CurrentHP, float MaxHP)
{
	// 이 위젯 컴포넌트가 표시하는 UMG 위젯 인스턴스를 가져옵니다.
	UUserWidget* WidgetInstance = GetUserWidgetObject();

	if (WidgetInstance)
	{
		// TODO: 여기에 실제 UMG 위젯 블루프린트에서 HP를 업데이트하는 함수를 호출합니다.
		// 예시: WidgetInstance->CallFunctionByName(TEXT("UpdateHP"), FString::Printf(TEXT("%f,%f"), CurrentHP, MaxHP));
		// 또는 UMG 위젯 블루프린트에서 C++로 노출된 함수를 직접 호출할 수 있습니다.
		// 예를 들어, UPROPERTY(BlueprintCallable) void UMyHPBarWidget::UpdateHPBar(float Current, float Max); 와 같은 함수가 있다면:
		// UMyHPBarWidget* HPBarWidget = Cast<UMyHPBarWidget>(WidgetInstance);
		// if (HPBarWidget) HPBarWidget->UpdateHPBar(CurrentHP, MaxHP);

		// 현재는 디버그 메시지로 대체합니다.
		UE_LOG(LogTemp, Log, TEXT("Monster HP Updated: Current=%.2f, Max=%.2f"), CurrentHP, MaxHP);
	}

	// HP가 0이 되면 위젯을 숨길 수 있습니다.
	SetVisibility(CurrentHP > 0.0f);
}
