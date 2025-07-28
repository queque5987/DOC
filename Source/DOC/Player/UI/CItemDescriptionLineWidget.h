#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "CItemDescriptionLineWidget.generated.h"

UCLASS()
class DOC_API UCItemDescriptionLineWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemStatusText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemStatusCompareText;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};