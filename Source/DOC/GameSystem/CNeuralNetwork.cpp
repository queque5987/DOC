#include "GameSystem/CNeuralNetwork.h"
//#include "Misc/Paths.h"
//#include "Logging/LogMacros.h"

UCNeuralNetwork::UCNeuralNetwork()
{
    ConstructorHelpers::FObjectFinder<UNNEModelData> ModelFinder(TEXT("/Game/Data/Models/xgboost_doc_model_V4_aug10_702.xgboost_doc_model_V4_aug10_702"));
    if (ModelFinder.Succeeded()) ModelDataAsset = ModelFinder.Object;
    AddToRoot();
}

UCNeuralNetwork::~UCNeuralNetwork()
{
    RemoveFromRoot();
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
    //Model = new IModelCPU
    Model = Runtime->CreateModelCPU(ModelDataAsset);
    

    if (!Model.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("CNeuralNetwork: Failed to create a model instance from the asset."));
        return false;
    }

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

    // Set Const Params
    InputTensorDescs = Model->GetInputTensorDescs();
    OutputTensorDescs = Model->GetOutputTensorDescs();

    //InputTensors.Shape = { 1, 100 };

    //InputBindings.SetNum(1);
    //InputShapes.SetNum(1);
    //InputShapes[0] = FTensorShape::MakeFromSymbolic(FSymbolicTensorShape::Make(InputTensors.Shape));

    //if (Model->SetInputTensorShapes(InputShapes) != 0)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("UCNeuralNetwork : Failed to set the input shapes"));
    //    return false;
    //}

    //OutputTensors.SetNum(OutputTensorDescs.Num());
    //OutputBindings.SetNum(OutputTensors.Num());
    //OutputShape = TArray<int32>(OutputTensorDescs[0].GetShape().GetData());

    //for (int32 i = 0; i < OutputTensors.Num(); i++)
    //{
    //    OutputTensors[i].Shape = OutputShape;
    //}
    //for (int32 i = 0; i < OutputTensors.Num(); i++)
    //{
    //    OutputBindings[i].SizeInBytes = OutputTensors[i].Data.Num() * sizeof(float);
    //}
    return true;
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
        InitializeModel();
        return;
    }

    // 2. Get model's input and output descriptions
    //InputTensorDescs = Model->GetInputTensorDescs();
    //OutputTensorDescs = Model->GetOutputTensorDescs();

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
    InputTensors.Shape = { 1, InputData.Num() };
    InputTensors.Data = InputData;
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

    if (Model.Get()->SetInputTensorShapes(InputShapes) != 0)
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
    if (Model.Get()->RunSync(InputBindings, OutputBindings) != 0)
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

void UCNeuralNetwork::RunInference(TArray<FPlayerTimeSeriesDataV3>& TimeSeriesDataArr, float& OutputMove)
{
    using namespace UE::NNECore;

    TArray<float> InputData = CreateFeaturesFromTimeSeries(TimeSeriesDataArr);

    // 1. Check if the model is initialized
    TArray<float> ResultData;
    if (!Model.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("CNeuralNetwork: RunInference called but the model is not initialized. Call InitializeModel() first."));
        InitializeModel();
        return;
    }

    // 2. Get model's input and output descriptions

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
    InputTensors.Shape = { 1, InputData.Num() };
    InputTensors.Data = InputData;
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

    if (Model.Get()->SetInputTensorShapes(InputShapes) != 0)
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
    if (Model.Get()->RunSync(InputBindings, OutputBindings) != 0)
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

TArray<float> UCNeuralNetwork::CreateFeaturesFromTimeSeries(FPlayerTimeSeriesDataV2& TimeSeriesData)
{
    TArray<float> Features;
    TArray<const TArray<float>*> FloatLagSources;
    FloatLagSources.Add(&TimeSeriesData.PlayerForwardRadian);
    FloatLagSources.Add(&TimeSeriesData.PlayerVelocity);
    FloatLagSources.Add(&TimeSeriesData.RelativeRadian);
    FloatLagSources.Add(&TimeSeriesData.RelativeDistance);
    FloatLagSources.Add(&TimeSeriesData.DistFromTop);
    FloatLagSources.Add(&TimeSeriesData.DistFromBottom);
    FloatLagSources.Add(&TimeSeriesData.DistFromLeft);
    FloatLagSources.Add(&TimeSeriesData.DistFromRight);
    FloatLagSources.Add(&TimeSeriesData.PlayerHP);
    FloatLagSources.Add(&TimeSeriesData.PlayerStamina);

    for (int32 depth = 0; depth < 10; depth++)
    {
        for (const TArray<float>* arriter : FloatLagSources)
        {
            Features.Add(arriter->IsValidIndex(depth) ? (*arriter)[depth] : 0.f);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("UCNeuralNetwork : CreateFeaturesFromTimeSeries : Loaded Features Num : %f"), Features.Num());
    return Features;
}

TArray<float> UCNeuralNetwork::CreateFeaturesFromTimeSeries(TArray<FPlayerTimeSeriesDataV3>& TimeSeriesDataArr)
{
    TArray<float> Features;
    Features.Reserve(MAX_BOSS_INFERENCE_TIMESERIESE_NUM * TimeSeriesDataArr.Num());
    for (auto& TimeSeriesData : TimeSeriesDataArr)
    {
        Features.Add(TimeSeriesData.PlayerForwardRadian);
        Features.Add(TimeSeriesData.PlayerVelocity_X);
        Features.Add(TimeSeriesData.PlayerVelocity_Y);
        Features.Add(TimeSeriesData.RelativeDistance);
        Features.Add(TimeSeriesData.DistFromTop);
        Features.Add(TimeSeriesData.DistFromBottom);
        Features.Add(TimeSeriesData.DistFromLeft);
        Features.Add(TimeSeriesData.DistFromRight);
        Features.Add(TimeSeriesData.PlayerHP);
        Features.Add(TimeSeriesData.PlayerStamina);
    }
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
