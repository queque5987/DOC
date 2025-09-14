#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NNECore.h"
#include "NNECoreRuntimeCPU.h"
#include "NNECoreModelData.h"
#include "CNeuralNetwork.generated.h"

class UNeuralNetwork;

UCLASS(Blueprintable, BlueprintType)
class DOC_API UCNeuralNetwork : public UObject
{
    GENERATED_BODY()

public:
    UCNeuralNetwork();

    UPROPERTY(EditAnywhere)
    UNNEModelData* ModelData;
    // Call this to load the model from the path defined in the .cpp
    UFUNCTION(BlueprintCallable, Category = "Neural Network")
    void LoadModelFromPath();

    // This will hold the network once loaded
    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Neural Network")
    //UNeuralNetwork* Network;

    TSharedPtr<UE::NNECore::IModelCPU> Model;

    TArray<UE::NNECore::FTensorBindingCPU> InputBindings;
    TArray<UE::NNECore::FTensorShape> InputShapes;
};
