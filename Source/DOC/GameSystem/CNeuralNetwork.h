#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NNECore.h"
#include "NNECoreRuntimeCPU.h"
#include "NNECoreModelData.h"
#include "Interfaces/CStageStructs.h"
#include "CNeuralNetwork.generated.h"

USTRUCT(BlueprintType)
struct FNeuralNetworkTensor
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite)
    TArray<int32> Shape = TArray<int32>();

    UPROPERTY(BlueprintReadWrite)
    TArray<float> Data = TArray<float>();
};

UCLASS(Blueprintable, BlueprintType)
class DOC_API UCNeuralNetwork : public UObject
{
    GENERATED_BODY()
    UCNeuralNetwork();
private:
    TWeakInterfacePtr<INNERuntimeCPU> Runtime = nullptr;
    TUniquePtr<UE::NNECore::IModelCPU> Model;
public:
    UPROPERTY(EditAnywhere, Category = "Neural Network|Setup")
    UNNEModelData* ModelDataAsset;

    UFUNCTION(BlueprintCallable, Category = "Neural Network")
    bool InitializeModel();

    UFUNCTION(BlueprintCallable, Category = "Neural Network|Inference")
    TArray<float> RunInference(const TArray<float>& FeatureVector);

    UFUNCTION(BlueprintCallable, Category = "Neural Network|Features")
    TArray<float> CreateFeaturesFromTimeSeries(FPlayerTimeSeriesData& TimeSeriesData, int32 Index);

private:
    float GetRollingMean(const TArray<int32>& Data, int32 Index, int32 Window);
    float GetRollingStd(const TArray<int32>& Data, int32 Index, int32 Window);

    TArray<int32> GetInputShape(int32 Index);
    static bool CreateTensor(TArray<int32> Shape, UPARAM(ref) FNeuralNetworkTensor& Tensor);
};
