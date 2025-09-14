#include "GameSystem/CNeuralNetwork.h"
#include "NNECoreRuntimeCPU.h"
#include "NNECoreModelData.h"
#include "Misc/Paths.h"
#include "Logging/LogMacros.h"

UCNeuralNetwork::UCNeuralNetwork()
{
    ConstructorHelpers::FObjectFinder<UNNEModelData> ModelFinder(TEXT("/Game/Data/Models/xgboost_doc_model.xgboost_doc_model"));
    if (ModelFinder.Succeeded()) ModelDataAsset = ModelFinder.Object;

}

bool UCNeuralNetwork::InitializeModel()
{
    using namespace UE::NNECore;

    // 1. Check if the model asset is assigned in the editor
    if (!ModelDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("CNeuralNetwork: ModelDataAsset is not assigned!"));
        return false;
    }

    // 2. Get the NNE CPU runtime
    TArrayView<TWeakInterfacePtr<INNERuntime>> Runtimes = GetAllRuntimes();
    if (Runtimes.Num() > 0 && Runtimes[0].Get() != nullptr)
    {
        Runtime = GetRuntime<INNERuntimeCPU>(Runtimes[0].Get()->GetRuntimeName());
    }
    if (!Runtime.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: NNERuntimeCPU could not be found."));
        return false;
    }

    // 3. Create the model instance from the asset
    Model = Runtime->CreateModelCPU(ModelDataAsset);
    if (!Model.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: Failed to create a model instance from the asset."));
        return false;
    }

    // (Optional) Log model's input and output tensor descriptions for debugging
    UE_LOG(LogTemp, Log, TEXT("CNeuralNetwork: Model '%s' initialized successfully."), *ModelDataAsset->GetName());
    for (const FTensorDesc& Desc : Model->GetInputTensorDescs())
    {
        FString ShapeStr;
        for (int32 Dim : Desc.GetShape().GetData())
        {
            ShapeStr += FString::Printf(TEXT("%d "), Dim);
        }
        UE_LOG(LogTemp, Log, TEXT("  - Input: Name=%s, Shape=[%s], Type=%d"), *Desc.GetName(), *ShapeStr, (int32)Desc.GetDataType());
    }
     for (const FTensorDesc& Desc : Model->GetOutputTensorDescs())
    {
        FString ShapeStr;
        for (int32 Dim : Desc.GetShape().GetData())
        {
            ShapeStr += FString::Printf(TEXT("%d "), Dim);
        }
        UE_LOG(LogTemp, Log, TEXT("  - Output: Name=%s, Shape=[%s], Type=%d"), *Desc.GetName(), *ShapeStr, (int32)Desc.GetDataType());
    }

    return true;
}

TArray<float> UCNeuralNetwork::RunInference(const TArray<float>& InputData)
{
    using namespace UE::NNECore;

    TArray<float> ResultData;
    // 1. Check if the model is initialized
    if (!Model.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("CNeuralNetwork: RunInference called but the model is not initialized. Call InitializeModel() first."));
        return {};
    }

    // 2. Get model's input and output descriptions
    TConstArrayView<FTensorDesc> InputTensorDescs = Model->GetInputTensorDescs();
    TConstArrayView<FTensorDesc> OutputTensorDescs = Model->GetOutputTensorDescs();

    if (InputTensorDescs.Num() == 0 || OutputTensorDescs.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: Model has no input or output tensors."));
        return {};
    }

    // 3. Validate input data size
    //TArray<int32> InputShapes = TArray<int32>(InputTensorDescs[0].GetShape().GetData());
    const FTensorDesc& InputDesc = InputTensorDescs[0];
    //int64 InputElementCount = 1;
    //for (int32 Dim : InputDesc.GetShape().GetData())
    //{
    //    InputElementCount *= Dim;
    //}

    //if (InputData.Num() != InputElementCount)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("CNeuralNetwork: Input data size mismatch. Model expects %lld elements, but %d were provided."), InputElementCount, InputData.Num());
    //    return {};
    //}
    TArray<int32> Shape = TArray<int32>(InputDesc.GetShape().GetData());
    int32 Volume = Shape[1];
    //for (int32 i = 0; i < Shape.Num(); i++)
    //{
    //    if (Shape[i] > 0) Volume *= Shape[i];
    //}
    //Volume *= InputData.Num();

    //// 4. Create input tensor and copy data
    TArray<FNeuralNetworkTensor> InputTensors;
    InputTensors.SetNum(InputData.Num());
    for (int32 i = 0; i < InputData.Num(); i++)
    {
        if (InputData[0].Num() == Volume)
        {
            InputTensors[i].Shape = {1, 35};
            InputTensors[i].Data.SetNum(Volume);
            InputTensors[i].Data = InputData[i];
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UCNeuralNetwork: Input Size Miss Matched"));
        }
    }
    //FMemory::Memcpy(InputTensors.GetData(), InputData.GetData(), InputData.Num() * sizeof(float));
    // 5. Prepare input and output tensor bindings
    TArray<FTensorBindingCPU> InputBindings;
    TArray<FTensorShape> InputShapes;
    InputBindings.Reset();
    InputBindings.SetNum(InputTensors.Num());
    InputShapes.Reset();
    InputShapes.SetNum(InputTensors.Num());
    for (int32 i = 0; i < InputData.Num(); i++)
    {
        InputBindings[i].Data = (void*)InputTensors[i].Data.GetData();
        InputBindings[i].SizeInBytes = InputTensors[i].Data.Num() * sizeof(float);
        InputShapes[i] = FTensorShape::MakeFromSymbolic(FSymbolicTensorShape::Make(InputTensors[i].Shape));
    }

    if (Model->SetInputTensorShapes(InputShapes) != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("UCNeuralNetwork : Failed to set the input shapes"));
        return {};
    }
    //InputBindings.Emplace(InputTensor.GetView());
    //TArray<FTensor> OutputTensors;
    //OutputTensors.SetNum(OutputTensorDescs.Num());
    //TArray<FTensorBindingCPU> OutputBindings;
    //for (int i = 0; i < OutputTensorDescs.Num(); ++i)
    //{
    //    OutputTensors[i] = FTensor(OutputTensorDescs[i].GetShape(), OutputTensorDescs[i].GetDataType());
    //    OutputBindings.Emplace(OutputTensors[i].GetView());
    //}
    TArray<FNeuralNetworkTensor> OutputTensors;
    TArray<FTensorBindingCPU> OutputBindings;

    OutputTensors.SetNum(OutputTensorDescs.Num());
    OutputBindings.SetNum(OutputTensors.Num());
    TArray<int32> OutputShape = TArray<int32>(OutputTensorDescs[0].GetShape().GetData());
    for (int32 i = 0; i < OutputTensors.Num(); i++)
    {
        OutputTensors[i].Shape = OutputShape;
        OutputTensors[i].Data.Add(0.f);
    }
    for (int32 i = 0; i < OutputTensors.Num(); i++)
    {
        OutputBindings[i].Data = (void*)OutputTensors[i].Data.GetData();
        OutputBindings[i].SizeInBytes = OutputTensors[i].Data.Num() * sizeof(float);
    }
    //// 6. Run inference
    if (Model->RunSync(InputBindings, OutputBindings) != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: Model inference run failed."));
    }
    if (OutputTensors[0].Data.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("CNeuralNetwork: Inferenced : %f."), OutputTensors[0].Data[0]);
    }
    //// 7. Extract and return the result from the first output tensor
    //const float* OutputDataPtr = OutputTensors[0].GetData<float>();
    //int64 OutputElementCount = 1;
    //for (int32 Dim : OutputTensors[0].GetShape().GetData())
    //{
    //    OutputElementCount *= Dim;
    //}
    //ResultData.Append(OutputDataPtr, OutputElementCount);
    //for (float R : ResultData)
    //{
    //    UE_LOG(LogTemp, Log, TEXT("Output : %f"), R);
    //}
    return ResultData;
}

TArray<int32> UCNeuralNetwork::GetInputShape(int32 Index)
{
    check(Model.IsValid())

    using namespace UE::NNECore;

    TConstArrayView<FTensorDesc> Desc = Model->GetInputTensorDescs();
    if (Index < 0 || Index >= Desc.Num())
    {
        return TArray<int32>();
    }

    return TArray<int32>(Desc[Index].GetShape().GetData());
}

bool UCNeuralNetwork::CreateTensor(TArray<int32> Shape, UPARAM(ref)FNeuralNetworkTensor& Tensor)
{
    if (Shape.Num() == 0)
    {
        return false;
    }

    int32 Volume = 1;
    for (int32 i = 0; i < Shape.Num(); i++)
    {
        if (Shape[i] < 1)
        {
            return false;
        }
        Volume *= Shape[i];
    }

    Tensor.Shape = Shape;
    Tensor.Data.SetNum(Volume);
    return true;
}
