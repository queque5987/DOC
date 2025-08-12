#include "Player/UI/CDamageWidget.h"
#include "Interfaces/IEnemyAIController.h"
#include "UObject/ConstructorHelpers.h"

UCDamageWidget::UCDamageWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    DamageTextures.SetNum(ATTACK_TYPE_NUM);

    ConstructorHelpers::FObjectFinder<UTexture2D> Texture1(TEXT("/Game/InventoryKit/Widgets/Common/Textures/ItemIcons/T_Dagger.T_Dagger"));
    if (Texture1.Succeeded())
    {
        DamageTextures[ATTACK_TYPE_MELLE] = Texture1.Object;
    }

    ConstructorHelpers::FObjectFinder<UTexture2D> Texture2(TEXT("/Game/InventoryKit/Widgets/Common/Textures/ItemIcons/T_BowAndArrow.T_BowAndArrow"));
    if (Texture2.Succeeded())
    {
        DamageTextures[ATTACK_TYPE_RANGED] = Texture2.Object;
    }

    ConstructorHelpers::FObjectFinder<UTexture2D> Texture3(TEXT("/Game/InventoryKit/Widgets/Common/Textures/ItemIcons/T_TiedScroll.T_TiedScroll"));
    if (Texture3.Succeeded())
    {
        DamageTextures[ATTACK_TYPE_MAGIC] = Texture3.Object;
    }

    ConstructorHelpers::FObjectFinder<UTexture2D> Texture4(TEXT("/Game/InventoryKit/Widgets/Common/Textures/ItemIcons/T_CrossedSword.T_CrossedSword"));
    if (Texture3.Succeeded())
    {
        DamageTextures[ATTACK_TYPE_COUNTER] = Texture4.Object;
    }
}

void UCDamageWidget::SetWidget(FDamageConfig DamageConfig)
{
    if (DamageConfig.bIsCrit)
    {
        CritDamageAmountText->SetText(FText::AsNumber(DamageConfig.Damage));
        CritDamageAmountText->SetColorAndOpacity(DamageConfig.DamageWidgetColor);
        CritDamageAmountText->SetVisibility(ESlateVisibility::Visible);
        DamageAmountText->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        DamageAmountText->SetText(FText::AsNumber(DamageConfig.Damage));
        DamageAmountText->SetColorAndOpacity(DamageConfig.DamageWidgetColor);
        DamageAmountText->SetVisibility(ESlateVisibility::Visible);
        CritDamageAmountText->SetVisibility(ESlateVisibility::Collapsed);
    }
	CritBG->SetVisibility(DamageConfig.bIsCrit ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    
    DamageIcon->SetColorAndOpacity(!DamageConfig.Instigator->Implements<UIEnemyAIController>() ? FLinearColor(1.f, 0.52f, 0.f, 1.f) : FLinearColor::White);
	SpawnStamp = DamageConfig.CausedTimeSeconds;

    if (DamageTextures.IsValidIndex(DamageConfig.AttackType))
    {
        DamageIcon->SetBrushFromTexture(DamageTextures[DamageConfig.AttackType]);
        DamageIcon_shadow->SetBrushFromTexture(DamageTextures[DamageConfig.AttackType]);
    }
    else
    {
        if (DamageTextures.Num() > 0)
        {
            DamageIcon->SetBrushFromTexture(DamageTextures[0]);
            DamageIcon_shadow->SetBrushFromTexture(DamageTextures[0]);
        }
    }
}
