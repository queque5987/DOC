#include "GameSystem/CNeuralNetwork.h"
#include "NNECoreRuntimeCPU.h"
#include "NNECoreModelData.h"
#include "Misc/Paths.h"
#include "Logging/LogMacros.h"

UCNeuralNetwork::UCNeuralNetwork()
{
    ConstructorHelpers::FObjectFinder<UNNEModelData> ModelFinder(TEXT("/Game/Data/Models/xgboost_doc_model_V2.xgboost_doc_model_V2"));
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

//TArray<float> UCNeuralNetwork::RunInference(const TArray<float>& InputData, float& OutputMove)
//{
//    using namespace UE::NNECore;
//
//    UE_LOG(LogTemp, Log, TEXT("CNeuralNetwork: RunInference called at %f."), GetWorld()->GetTimeSeconds());
//    // 1. Check if the model is initialized
//    TArray<float> ResultData;
//    if (!Model.IsValid())
//    {
//        UE_LOG(LogTemp, Warning, TEXT("CNeuralNetwork: RunInference called but the model is not initialized. Call InitializeModel() first."));
//        return {};
//    }
//
//    // 2. Get model's input and output descriptions
//    TConstArrayView<FTensorDesc> InputTensorDescs = Model->GetInputTensorDescs();
//    TConstArrayView<FTensorDesc> OutputTensorDescs = Model->GetOutputTensorDescs();
//
//    if (InputTensorDescs.Num() == 0 || OutputTensorDescs.Num() == 0)
//    {
//        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: Model has no input or output tensors."));
//        return {};
//    }
//
//    // 3. Validate input data size
//    const FTensorDesc& InputDesc = InputTensorDescs[0];
//    TArray<int32> Shape = TArray<int32>(InputDesc.GetShape().GetData());
//    int32 Volume = Shape[1];
//
//    //// 4. Create input tensor and copy data
//    FNeuralNetworkTensor InputTensors;
//    InputTensors.Shape = { 1, 35 };
//    InputTensors.Data = InputData;
//    //FMemory::Memcpy(InputTensors.GetData(), InputData.GetData(), InputData.Num() * sizeof(float));
//    // 5. Prepare input and output tensor bindings
//    TArray<FTensorBindingCPU> InputBindings;
//    TArray<FTensorShape> InputShapes;
//    InputBindings.Reset();
//    InputBindings.SetNum(1);
//    InputShapes.Reset();
//    InputShapes.SetNum(1);
//
//    InputBindings[0].Data = (void*)InputTensors.Data.GetData();
//    InputBindings[0].SizeInBytes = InputTensors.Data.Num() * sizeof(float);
//    InputShapes[0] = FTensorShape::MakeFromSymbolic(FSymbolicTensorShape::Make(InputTensors.Shape));
//
//    if (Model->SetInputTensorShapes(InputShapes) != 0)
//    {
//        UE_LOG(LogTemp, Error, TEXT("UCNeuralNetwork : Failed to set the input shapes"));
//        return {};
//    }
//    //InputBindings.Emplace(InputTensor.GetView());
//    //TArray<FTensor> OutputTensors;
//    //OutputTensors.SetNum(OutputTensorDescs.Num());
//    //TArray<FTensorBindingCPU> OutputBindings;
//    //for (int i = 0; i < OutputTensorDescs.Num(); ++i)
//    //{
//    //    OutputTensors[i] = FTensor(OutputTensorDescs[i].GetShape(), OutputTensorDescs[i].GetDataType());
//    //    OutputBindings.Emplace(OutputTensors[i].GetView());
//    //}
//    TArray<FNeuralNetworkTensor> OutputTensors;
//    TArray<FTensorBindingCPU> OutputBindings;
//
//    OutputTensors.SetNum(OutputTensorDescs.Num());
//    OutputBindings.SetNum(OutputTensors.Num());
//    TArray<int32> OutputShape = TArray<int32>(OutputTensorDescs[0].GetShape().GetData());
//    for (int32 i = 0; i < OutputTensors.Num(); i++)
//    {
//        OutputTensors[i].Shape = OutputShape;
//        OutputTensors[i].Data.Add(0.f);
//    }
//    for (int32 i = 0; i < OutputTensors.Num(); i++)
//    {
//        OutputBindings[i].Data = (void*)OutputTensors[i].Data.GetData();
//        OutputBindings[i].SizeInBytes = OutputTensors[i].Data.Num() * sizeof(float);
//    }
//    //// 6. Run inference
//    if (Model->RunSync(InputBindings, OutputBindings) != 0)
//    {
//        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: Model inference run failed."));
//    }
//    if (OutputTensors[0].Data.Num() > 0)
//    {
//        UE_LOG(LogTemp, Log, TEXT("CNeuralNetwork: Inferenced : %f."), OutputTensors[0].Data[0]);
//        UE_LOG(LogTemp, Log, TEXT("CNeuralNetwork: RunInference Inferenced at %f."), GetWorld()->GetTimeSeconds());
//        OutputMove = OutputTensors[0].Data[0];
//    }
//    return ResultData;
//}

void UCNeuralNetwork::RunInference(FPlayerTimeSeriesData& TimeSeriesData, float& OutputMove)
{
    using namespace UE::NNECore;

    TArray<float> InputData = CreateFeaturesFromTimeSeries(TimeSeriesData, TimeSeriesData.PlayerButtonSeries.Num() - 1);

    //if (GetWorld()) UE_LOG(LogTemp, Log, TEXT("CNeuralNetwork: RunInference called at %f."), GetWorld()->GetTimeSeconds());
    // 1. Check if the model is initialized
    TArray<float> ResultData;
    if (!Model.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("CNeuralNetwork: RunInference called but the model is not initialized. Call InitializeModel() first."));
        return;
    }

    // 2. Get model's input and output descriptions
    TConstArrayView<FTensorDesc> InputTensorDescs = Model->GetInputTensorDescs();
    TConstArrayView<FTensorDesc> OutputTensorDescs = Model->GetOutputTensorDescs();

    if (InputTensorDescs.Num() == 0 || OutputTensorDescs.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: Model has no input or output tensors."));
        return;
    }

    // 3. Validate input data size
    const FTensorDesc& InputDesc = InputTensorDescs[0];
    TArray<int32> Shape = TArray<int32>(InputDesc.GetShape().GetData());
    int32 Volume = Shape[1];

    //// 4. Create input tensor and copy data
    FNeuralNetworkTensor InputTensors;
    InputTensors.Shape = { 1, 35 };
    InputTensors.Data = InputData;
    //FMemory::Memcpy(InputTensors.GetData(), InputData.GetData(), InputData.Num() * sizeof(float));
    // 5. Prepare input and output tensor bindings
    TArray<FTensorBindingCPU> InputBindings;
    TArray<FTensorShape> InputShapes;
    InputBindings.Reset();
    InputBindings.SetNum(1);
    InputShapes.Reset();
    InputShapes.SetNum(1);

    InputBindings[0].Data = (void*)InputTensors.Data.GetData();
    InputBindings[0].SizeInBytes = InputTensors.Data.Num() * sizeof(float);
    InputShapes[0] = FTensorShape::MakeFromSymbolic(FSymbolicTensorShape::Make(InputTensors.Shape));

    if (Model->SetInputTensorShapes(InputShapes) != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("UCNeuralNetwork : Failed to set the input shapes"));
        return;
    }

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
        UE_LOG(LogTemp, Log, TEXT("CNeuralNetwork: RunInference Inferenced at %f."), GetWorld()->GetTimeSeconds());
        OutputMove = OutputTensors[0].Data[0];
    }
}

float UCNeuralNetwork::GetRollingMean(const TArray<int32>& Data, int32 Index, int32 Window)
{
    if (Index < 0 || Window <= 0 || Index - Window + 1 < 0 || Index >= Data.Num())
    {
        return 0.f;
    }

    float Sum = 0.f;
    for (int32 i = 0; i < Window; ++i)
    {
        Sum += static_cast<float>(Data[Index - i]);
    }
    return Sum / static_cast<float>(Window);
}

float UCNeuralNetwork::GetRollingStd(const TArray<int32>& Data, int32 Index, int32 Window)
{
    if (Index < 0 || Window <= 1 || Index - Window + 1 < 0 || Index >= Data.Num())
    {
        return 0.f; // Not enough data for the window (need at least 2 for std dev)
    }

    float Mean = GetRollingMean(Data, Index, Window);
    float SumSquaredDiff = 0.f;
    for (int32 i = 0; i < Window; ++i)
    {
        float Diff = static_cast<float>(Data[Index - i]) - Mean;
        SumSquaredDiff += Diff * Diff;
    }
    return FMath::Sqrt(SumSquaredDiff / static_cast<float>(Window - 1)); // Sample standard deviation
}

void UCNeuralNetwork::RunInference(FPlayerTimeSeriesDataV2& TimeSeriesData, float& OutputMove)
{
    using namespace UE::NNECore;

    TArray<float> InputData = CreateFeaturesFromTimeSeries(TimeSeriesData);

    // 1. Check if the model is initialized
    TArray<float> ResultData;
    if (!Model.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("CNeuralNetwork: RunInference called but the model is not initialized. Call InitializeModel() first."));
        return;
    }

    // 2. Get model's input and output descriptions
    TConstArrayView<FTensorDesc> InputTensorDescs = Model->GetInputTensorDescs();
    TConstArrayView<FTensorDesc> OutputTensorDescs = Model->GetOutputTensorDescs();

    if (InputTensorDescs.Num() == 0 || OutputTensorDescs.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: Model has no input or output tensors."));
        return;
    }

    // 3. Validate input data size
    const FTensorDesc& InputDesc = InputTensorDescs[0];
    TArray<int32> Shape = TArray<int32>(InputDesc.GetShape().GetData());
    int32 Volume = Shape[1];

    //// 4. Create input tensor and copy data
    FNeuralNetworkTensor InputTensors;
    InputTensors.Shape = { 1, InputData.Num()};
    InputTensors.Data = InputData;
    //FMemory::Memcpy(InputTensors.GetData(), InputData.GetData(), InputData.Num() * sizeof(float));
    // 5. Prepare input and output tensor bindings
    TArray<FTensorBindingCPU> InputBindings;
    TArray<FTensorShape> InputShapes;
    InputBindings.Reset();
    InputBindings.SetNum(1);
    InputShapes.Reset();
    InputShapes.SetNum(1);

    InputBindings[0].Data = (void*)InputTensors.Data.GetData();
    InputBindings[0].SizeInBytes = InputTensors.Data.Num() * sizeof(float);
    InputShapes[0] = FTensorShape::MakeFromSymbolic(FSymbolicTensorShape::Make(InputTensors.Shape));

    if (Model->SetInputTensorShapes(InputShapes) != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("UCNeuralNetwork : Failed to set the input shapes"));
        return;
    }

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
        UE_LOG(LogTemp, Log, TEXT("CNeuralNetwork: RunInference Inferenced at %f."), GetWorld()->GetTimeSeconds());
        OutputMove = OutputTensors[0].Data[0];
    }
}

TArray<float> UCNeuralNetwork::CreateFeaturesFromTimeSeries(FPlayerTimeSeriesData& Data, int32 Index)
{
    TArray<float> Features;

    Features.Add(Data.RelativeDistance[Index]);
    Features.Add(Data.DistFromTop[Index]);
    Features.Add(Data.DistFromBottom[Index]);
    Features.Add(Data.DistFromLeft[Index]);
    Features.Add(Data.DistFromRight[Index]);
    Features.Add(Data.PlayerHP[Index]);
    Features.Add(Data.PlayerStamina[Index]);

    // lags
    for (int32 i = 2; i <= 4; ++i)
    {
        int32 LagIndex = -i;
        Features.Add(Data.PlayerButtonSeries.IsValidIndex(LagIndex) ? static_cast<float>(Data.PlayerButtonSeries[LagIndex]) : 0.f);
    }

    // Other float TArray lags
    TArray<TArray<float>*> FloatLagSources;
    FloatLagSources.Add(&(Data.RelativeDistance));
    FloatLagSources.Add(&(Data.DistFromTop));
    FloatLagSources.Add(&(Data.DistFromBottom));
    FloatLagSources.Add(&(Data.DistFromLeft));
    FloatLagSources.Add(&(Data.DistFromRight));
    FloatLagSources.Add(&(Data.PlayerHP));
    FloatLagSources.Add(&(Data.PlayerStamina));

    for (const auto& SourceTuple : FloatLagSources)
    {
        const TArray<float>* CurrentSource = SourceTuple;
        //const FString& CurrentSourceName = SourceTuple.Get<1>();

        for (int32 i = 2; i <= 4; ++i)
        {
            int32 LagIndex = -i;
            if (CurrentSource->IsValidIndex(LagIndex))
            {
                Features.Add(CurrentSource->IsValidIndex(LagIndex) ? (*CurrentSource)[LagIndex] : 0.f);
            }
        }
    }

    Features.Add(GetRollingMean(Data.PlayerButtonSeries, Index - 1, 5));
    Features.Add(GetRollingStd(Data.PlayerButtonSeries, Index - 1, 5));
    Features.Add(GetRollingMean(Data.PlayerButtonSeries, Index - 1, 10));
    Features.Add(GetRollingStd(Data.PlayerButtonSeries, Index - 1, 10));

    return Features;
}

TArray<float> UCNeuralNetwork::CreateFeaturesFromTimeSeries(FPlayerTimeSeriesDataV2& TimeSeriesData)
{
    TArray<float> Features;
    TArray<const TArray<float>*> FloatLagSources;
    FloatLagSources(&TimeSeriesData.PlayerForwardRadian);
    FloatLagSources(&TimeSeriesData.PlayerVelocity);
    FloatLagSources(&TimeSeriesData.RelativeRadian);
    FloatLagSources(&TimeSeriesData.RelativeDistance);
    FloatLagSources(&TimeSeriesData.DistFromTop);
    FloatLagSources(&TimeSeriesData.DistFromBottom);
    FloatLagSources(&TimeSeriesData.DistFromLeft);
    FloatLagSources(&TimeSeriesData.DistFromRight);
    FloatLagSources(&TimeSeriesData.PlayerHP);
    FloatLagSources(&TimeSeriesData.PlayerStamina);

    for (int32 depth = 0; depth++; depth < 10)
    {
        for (const TArray<float>* arriter : FloatLagSources)
        {
            Features.Add(arriter->IsValidIndex(depth) ? (*arriter)[depth] : 0.f);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("UCNeuralNetwork : CreateFeaturesFromTimeSeries : Loaded Features Num : %f"), Features.Num());
    return Features;
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
