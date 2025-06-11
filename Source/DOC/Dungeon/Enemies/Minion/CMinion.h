#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CMinion.generated.h"

UCLASS()
class DOC_API ACMinion : public ACharacter
{
	GENERATED_BODY()

public:
	ACMinion();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
