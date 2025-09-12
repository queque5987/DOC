#include "Player/CHttpComponent.h"
#include "HttpFwd.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IPlayerOnStage.h"
#include "JsonUtilities.h"

UCHttpComponent::UCHttpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCHttpComponent::BeginPlay()
{
	Super::BeginPlay();

	Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UCHttpComponent::OnResponseReceived);
	Request->SetURL(TEXT("http://127.0.0.1:8000"));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}


void UCHttpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bRecordTimeSeriesData) return;


}

void UCHttpComponent::SendRequest()
{
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	Writer->WriteObjectStart();

	Writer->WriteArrayStart(TEXT("TimeStamp"));
	for (float value : PlayerTimeSeriesData.TimeStamp)
	{
		Writer->WriteValue(value);
	}
	Writer->WriteArrayEnd();

	// PlayerButtonSeries
	Writer->WriteArrayStart(TEXT("PlayerButtonSeries"));
	for (int32 value : PlayerTimeSeriesData.PlayerButtonSeries)
	{
		Writer->WriteValue(value);
	}
	Writer->WriteArrayEnd();

	// RelativeDistance
	Writer->WriteArrayStart(TEXT("RelativeDistance"));
	for (float value : PlayerTimeSeriesData.RelativeDistance)
	{
		Writer->WriteValue(value);
	}
	Writer->WriteArrayEnd();


	Writer->WriteArrayStart(TEXT("Dist_from_Top"));
	for (float value : PlayerTimeSeriesData.DistFromTop)
	{
		Writer->WriteValue(value);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("Dist_from_Bottom"));
	for (float value : PlayerTimeSeriesData.DistFromBottom)
	{
		Writer->WriteValue(value);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("Dist_from_Left"));
	for (float value : PlayerTimeSeriesData.DistFromLeft)
	{
		Writer->WriteValue(value);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("Dist_from_Right"));
	for (float value : PlayerTimeSeriesData.DistFromRight)
	{
		Writer->WriteValue(value);
	}
	Writer->WriteArrayEnd();

	// PlayerHP
	Writer->WriteArrayStart(TEXT("PlayerHP"));
	for (float value : PlayerTimeSeriesData.PlayerHP)
	{
		Writer->WriteValue(value);
	}
	Writer->WriteArrayEnd();

	// PlayerStamina
	Writer->WriteArrayStart(TEXT("PlayerStamina"));
	for (float value : PlayerTimeSeriesData.PlayerStamina)
	{
		Writer->WriteValue(value);
	}
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

	PlayerTimeSeriesData = FPlayerTimeSeriesData();
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
	//UE_LOG(LogTemp, Warning, TEXT("Response Content: %s"), *ResponseContent);
}

void UCHttpComponent::AddTimeSeriesData(int32 CurrButton, float CurrRelativeDist, float DistFromTop, float DistFromBot, float DistFromLeft, float DistFromRight, float PlayerHP, float PlayerStamina)
{
	if (GetWorld() == nullptr) return;
	PlayerTimeSeriesData.TimeStamp.Add(GetWorld()->GetTimeSeconds());
	PlayerTimeSeriesData.PlayerButtonSeries.Add(CurrButton);
	PlayerTimeSeriesData.RelativeDistance.Add(CurrRelativeDist);
	PlayerTimeSeriesData.DistFromTop.Add(DistFromTop);
	PlayerTimeSeriesData.DistFromBottom.Add(DistFromBot);
	PlayerTimeSeriesData.DistFromLeft.Add(DistFromLeft);
	PlayerTimeSeriesData.DistFromRight.Add(DistFromRight);
	PlayerTimeSeriesData.PlayerHP.Add(PlayerHP);
	PlayerTimeSeriesData.PlayerStamina.Add(PlayerStamina);
}

