#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NNECore.h"
#include "NNECoreRuntimeCPU.h"
#include "NNECoreModelData.h"
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
    // NNE runtime instance, created during initialization
    TWeakInterfacePtr<INNERuntimeCPU> Runtime = nullptr;

    // NNE model instance, created during initialization
    TUniquePtr<UE::NNECore::IModelCPU> Model;
    //TArray<UE::NNECore::FTensorShape> InputShapes;
public:
    // In the Unreal Editor, assign the UNNEModelData asset that you imported from the .onnx file.
    UPROPERTY(EditAnywhere, Category = "Neural Network|Setup")
    UNNEModelData* ModelDataAsset;

    /**
     * Initializes the NNE runtime and loads the model from the assigned ModelDataAsset.
     * This must be called before running inference.
     * @return True if initialization was successful, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Neural Network")
    bool InitializeModel();

    /**
     * Runs inference on the provided input data.
     * The input data must be a flattened 1D array containing all the features in the correct order.
     * @param InputData A 1D array of floats representing the input tensor.
     * @return A 1D array of floats representing the model's output. Returns an empty array on failure.
     */
    //UFUNCTION(BlueprintCallable, Category = "Neural Network")
    TArray<float> RunInference(const TArray<TArray<float>>& InputData);

private:
    TArray<int32> GetInputShape(int32 Index);
    static bool CreateTensor(TArray<int32> Shape, UPARAM(ref) FNeuralNetworkTensor& Tensor);
};
