#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "Json.h"
#include "CHttpComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOC_API UCHttpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCHttpComponent();

protected:
	virtual void BeginPlay() override;
	
public:	
	FHttpModule* Http;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//void SendRequest()
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};