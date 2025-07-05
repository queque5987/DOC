#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h" // UUserWidget을 사용하기 위해 추가
#include "CMonsterHP.generated.h"

UCLASS()
class DOC_API UCMonsterHP : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UCMonsterHP();

	// 언리얼 에디터에서 이 위젯 컴포넌트가 표시할 UMG 위젯 클래스를 지정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HPWidgetClass;

	// 몬스터의 현재 HP와 최대 HP를 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetMonsterHP(float CurrentHP, float MaxHP);
};
