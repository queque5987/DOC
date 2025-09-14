#include "GameSystem/CNeuralNetwork.h"
#include "NeuralNetwork.h"

UCNeuralNetwork::UCNeuralNetwork()
{
    //Network = nullptr;
    ConstructorHelpers::FObjectFinder<UNNEModelData> ModelDataFinder(TEXT("/Game/Data/Models/xgboost_doc_model.xgboost_doc_model"));
    if (ModelDataFinder.Succeeded()) ModelData = ModelDataFinder.Object;
}

void UCNeuralNetwork::LoadModelFromPath()
{
    using namespace UE::NNECore;

    TArray<FString> Results;
    TArrayView<TWeakInterfacePtr<INNERuntime>> Runtimes = GetAllRuntimes();
    for (int32 i = 0; i < Runtimes.Num(); i++)
    {
        if (Runtimes[i].IsValid() && Cast<INNERuntimeCPU>(Runtimes[i].Get()))
        {
            Results.Add(Runtimes[i]->GetRuntimeName());
        }
    }
    TWeakInterfacePtr<INNERuntimeCPU> Runtime = GetRuntime<INNERuntimeCPU>(Results[0]);
    if (!Runtime.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("No CPU runtime '%s' found"), *Results[0]);
        return;
    }
    TUniquePtr<IModelCPU> UniqueModel = Runtime->CreateModelCPU(ModelData);
    if (!UniqueModel.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Could not create the CPU model"));
        return;
    }
    UCNeuralNetwork* Result = NewObject<UCNeuralNetwork>(this);
    if (Result)
    {
        Result->Model = TSharedPtr<IModelCPU>(UniqueModel.Release());
        int32 NumInputs = Result->Model->GetInputTensorDescs().Num();
        TConstArrayView<FTensorDesc> Desc = Result->Model->GetInputTensorDescs();

        UE_LOG(LogTemp, Log, TEXT("input shape Num : %d"), NumInputs);
        for (int32 i = 0; i < NumInputs; i++)
        {
            TArray<int32> shapes = TArray<int32>(Desc[i].GetShape().GetData());
            for (int32 shape : shapes)
            {
                UE_LOG(LogTemp, Log, TEXT("Shape [%d] : %d"), i, shape);
            }
        }
    }
}
