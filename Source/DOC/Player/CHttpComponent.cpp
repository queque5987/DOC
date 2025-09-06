#include "Player/CHttpComponent.h"
#include "HttpFwd.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

UCHttpComponent::UCHttpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCHttpComponent::BeginPlay()
{
	Super::BeginPlay();

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	Writer->WriteObjectStart();

	Writer->WriteValue(TEXT("TimeStamp"), 1818);
	Writer->WriteArrayStart(TEXT("TimeSeries"));
	Writer->WriteValue(1);
	Writer->WriteValue(2);
	Writer->WriteValue(3);
	Writer->WriteValue(4);
	Writer->WriteValue(5);
	Writer->WriteArrayEnd();

	Writer->WriteObjectEnd();
	Writer->Close();
	UE_LOG(LogTemp, Warning, TEXT("Created JSON: %s"), *OutputString);

	Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UCHttpComponent::OnResponseReceived);
	Request->SetURL(TEXT("http://127.0.0.1:8000/data"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
}


void UCHttpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCHttpComponent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HTTP request failed. URL: %s"), *Request->GetURL());
		return;
	}
	int32 StatusCode = Response->GetResponseCode();

	FString ResponseContent = Response->GetContentAsString();

	UE_LOG(LogTemp, Warning, TEXT("HTTP request completed. Status Code: %d, URL: %s"), StatusCode, *Request->GetURL());
	UE_LOG(LogTemp, Warning, TEXT("Response Content: %s"), *ResponseContent);
}

