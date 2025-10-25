#include "Player/CHttpComponent.h"
#include "HttpFwd.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IPlayerOnStage.h"
#include "JsonUtilities.h"

UCHttpComponent::UCHttpComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

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

//void UCHttpComponent::SendRequest()
//{
//	FString OutputString;
//	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
//	Writer->WriteObjectStart();
//
//	Writer->WriteArrayStart(TEXT("TimeStamp"));
//	for (float value : PlayerTimeSeriesDataV2.TimeStamp)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("PlayerForwardRadian"));
//	for (float value : PlayerTimeSeriesDataV2.PlayerForwardRadian)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("PlayerVelocity"));
//	for (float value : PlayerTimeSeriesDataV2.PlayerVelocity)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("PlayerRelativeDirectionRadian"));
//	for (float value : PlayerTimeSeriesDataV2.RelativeRadian)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("RelativeDistance"));
//	for (float value : PlayerTimeSeriesDataV2.RelativeDistance)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("Dist_from_Top"));
//	for (float value : PlayerTimeSeriesDataV2.DistFromTop)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("Dist_from_Bottom"));
//	for (float value : PlayerTimeSeriesDataV2.DistFromBottom)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("Dist_from_Left"));
//	for (float value : PlayerTimeSeriesDataV2.DistFromLeft)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("Dist_from_Right"));
//	for (float value : PlayerTimeSeriesDataV2.DistFromRight)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("PlayerHP"));
//	for (float value : PlayerTimeSeriesDataV2.PlayerHP)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteArrayStart(TEXT("PlayerStamina"));
//	for (float value : PlayerTimeSeriesDataV2.PlayerStamina)
//	{
//		Writer->WriteValue(value);
//	}
//	Writer->WriteArrayEnd();
//
//	Writer->WriteObjectEnd();
//	Writer->Close();
//	UE_LOG(LogTemp, Warning, TEXT("Created JSON for V2: %s"), *OutputString);
//
//	Http = &FHttpModule::Get();
//	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
//	Request->OnProcessRequestComplete().BindUObject(this, &UCHttpComponent::OnResponseReceived);
//	Request->SetURL(TEXT("http://127.0.0.1:8000/data"));
//	Request->SetVerb(TEXT("POST"));
//	Request->SetHeader("Content-Type", TEXT("application/json"));
//	Request->SetContentAsString(OutputString);
//	Request->ProcessRequest();
//
//	PlayerTimeSeriesDataV2 = FPlayerTimeSeriesDataV2();
//}

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

//void UCHttpComponent::AddTimeSeriesData(int32 CurrButton, float CurrRelativeDist, float DistFromTop, float DistFromBot, float DistFromLeft, float DistFromRight, float PlayerHP, float PlayerStamina)
//{
//	if (GetWorld() == nullptr) return;
//	PlayerTimeSeriesData.TimeStamp.Add(GetWorld()->GetTimeSeconds());
//	PlayerTimeSeriesData.PlayerButtonSeries.Add(CurrButton);
//	PlayerTimeSeriesData.RelativeDistance.Add(CurrRelativeDist);
//	PlayerTimeSeriesData.DistFromTop.Add(DistFromTop);
//	PlayerTimeSeriesData.DistFromBottom.Add(DistFromBot);
//	PlayerTimeSeriesData.DistFromLeft.Add(DistFromLeft);
//	PlayerTimeSeriesData.DistFromRight.Add(DistFromRight);
//	PlayerTimeSeriesData.PlayerHP.Add(PlayerHP);
//	PlayerTimeSeriesData.PlayerStamina.Add(PlayerStamina);
//}
//
//void UCHttpComponent::AddTimeSeriesData(FVector PlayerForwardVector, float PlayerVelocity, FVector PlayerMovingDirectionVector, FVector RelativeDirectionVector, float RelativeDistance, float DistFromTop, float DistFromBot, float DistFromLeft, float DistFromRight, float PlayerHP, float PlayerStamina)
//{
//	if (GetWorld() == nullptr) return;
//	
//	FVector Forward = PlayerForwardVector;
//	FVector RelativeDirection = RelativeDirectionVector;
//	float Rad_Forward = FMath::Atan2(PlayerForwardVector.Y, PlayerForwardVector.X);
//	float Rad_RelativeDir = FMath::Atan2(RelativeDirectionVector.Y, RelativeDirectionVector.X);
//
//	PlayerTimeSeriesDataV2.TimeStamp.Add(GetWorld()->GetTimeSeconds());
//	//PlayerTimeSeriesDataV2.PlayerForwardVector.Add(PlayerForwardVector);
//	PlayerTimeSeriesDataV2.PlayerForwardRadian.Add(Rad_Forward);
//	PlayerTimeSeriesDataV2.PlayerVelocity.Add(PlayerVelocity);
//	//PlayerTimeSeriesDataV2.RelativeVector.Add(RelativeDirectionVector);
//	PlayerTimeSeriesDataV2.RelativeRadian.Add(Rad_RelativeDir);
//	PlayerTimeSeriesDataV2.RelativeDistance.Add(RelativeDistance);
//	PlayerTimeSeriesDataV2.DistFromTop.Add(DistFromTop);
//	PlayerTimeSeriesDataV2.DistFromBottom.Add(DistFromBot);
//	PlayerTimeSeriesDataV2.DistFromLeft.Add(DistFromLeft);
//	PlayerTimeSeriesDataV2.DistFromRight.Add(DistFromRight);
//	PlayerTimeSeriesDataV2.PlayerHP.Add(PlayerHP);
//	PlayerTimeSeriesDataV2.PlayerStamina.Add(PlayerStamina);
//}
//
//void UCHttpComponent::AddTimeSeriesData(TDoubleLinkedList<FPlayerTimeSeriesDataV3>* TimeSerieseDataLL)
//{
//	TArray<FPlayerTimeSeriesDataV3> tempDataArr;
//	for (auto Iter = TimeSerieseDataLL->GetHead(); Iter; Iter = Iter->GetNextNode())
//	{
//		FPlayerTimeSeriesDataV3& Data = Iter->GetValue();
//		FPlayerTimeSeriesDataV3 C_Data = Data;
//		tempDataArr.Add(C_Data);
//	}
//	if (tempDataArr.Num() != 10)
//	{
//		UE_LOG(LogTemp, Error, TEXT("UCHttpComponent::AddTimeSeriesData : TimeSerieseData Num Is Not 10"));
//	}
//	PlayerTimeSeriesDataV3_Container.Add(tempDataArr);
//}

void UCHttpComponent::Post_TimeSeriesData(TDoubleLinkedList<FPlayerTimeSeriesDataV3>* TimeSerieseDataLL)
{
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	Writer->WriteObjectStart();

	Writer->WriteArrayStart(TEXT("TimeStamp"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.TimeStamp);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("PlayerForwardRadian"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.PlayerForwardRadian);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("PlayerVelocity"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.PlayerVelocity);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("PlayerRelativeDirectionRadian"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.RelativeRadian);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("RelativeDistance"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.RelativeDistance);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("Dist_from_Top"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.DistFromTop);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("Dist_from_Bottom"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.DistFromBottom);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("Dist_from_Left"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.DistFromLeft);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("Dist_from_Right"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.DistFromRight);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("PlayerHP"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.PlayerHP);
	}
	Writer->WriteArrayEnd();

	Writer->WriteArrayStart(TEXT("PlayerStamina"));
	for (auto TimeSerieseData : (*TimeSerieseDataLL))
	{
		Writer->WriteValue(TimeSerieseData.PlayerStamina);
	}
	Writer->WriteArrayEnd();

	Writer->WriteObjectEnd();
	Writer->Close();
	UE_LOG(LogTemp, Warning, TEXT("Created JSON for V3: %s"), *OutputString);

	Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UCHttpComponent::OnResponseReceived);
	Request->SetURL(TEXT("http://127.0.0.1:8000/data"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
}

