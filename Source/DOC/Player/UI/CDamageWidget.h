#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PCH.h"
#include "Interfaces/CStageStructs.h"
#include "CDamageWidget.generated.h"

UCLASS()
class DOC_API UCDamageWidget : public UUserWidget
{
	GENERATED_BODY()
	UCDamageWidget(const FObjectInitializer& ObjectInitializer);
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageAmountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CritDamageAmountText;

	UPROPERTY(meta = (BindWidget))
	class UImage* CritBG;

	UPROPERTY(meta = (BindWidget))
	class UImage* DamageIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* DamageIcon_shadow;

	TArray<UTexture2D*> DamageTextures;

	float SpawnStamp = 0.f;

	void SetWidget(FDamageConfig DamageConfig);
};
