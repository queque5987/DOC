#include "DOCEditor.h"
#include "Engine/StaticMesh.h"
#include "AssetRegistryModule.h"
#include "IMeshReductionManagerModule.h"
#include "IMeshReductionInterfaces.h"
#include "MeshDescription.h"
#include "OverlappingCorners.h"
#include "StaticMeshResources.h"
#include "StaticMeshOperations.h"
#include "StaticMeshAttributes.h"
#include "StaticMeshDescription.h"

IMPLEMENT_MODULE(FDOCEditorModule, DOCEditor)

void FDOCEditorModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("DOCEditor module Started"));
	return;
	FString Path = TEXT("/Game/Dungeon/Meshes/Ceiling");
	GenerateStaticMeshLOD(Path);
	Path = TEXT("/Game/Dungeon/Meshes/Floor");
	GenerateStaticMeshLOD(Path);
	Path = TEXT("/Game/Dungeon/Meshes/Wall");
	GenerateStaticMeshLOD(Path);
}

void FDOCEditorModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("DOCEditor module Ended"));
}

void FDOCEditorModule::GenerateStaticMeshLOD(FString& FolderPath)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetDataArray;
	FARFilter Filter;
	Filter.PackagePaths.Add(*FolderPath);
	Filter.ClassNames.Add(UStaticMesh::StaticClass()->GetFName());
	Filter.bRecursivePaths = true;
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataArray);

	//IMeshReduction* MeshReduction = FModuleManager::Get().LoadModuleChecked<IMeshReductionManagerModule>("MeshReductionInterface").GetStaticMeshReductionInterface();

	if (MeshReduction == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh Reduction Module UnAvilable"));
		return;
	}
	for (const FAssetData& AssetData : AssetDataArray)
	{
		UStaticMesh* StaticMesh = Cast<UStaticMesh>(AssetData.GetAsset());
		if (StaticMesh == nullptr) continue;
		UE_LOG(LogTemp, Log, TEXT("StaticMesh : %s Found"), *StaticMesh->GetName());
		StaticMesh->Modify();

		StaticMesh->SetNumSourceModels(3);

		if (StaticMesh->GetRenderData() != nullptr)
		{
			int32 T = StaticMesh->GetRenderData()->LODResources[0].GetNumTriangles();
			int32 V = StaticMesh->GetRenderData()->LODResources[0].GetNumVertices();
			
			StaticMesh->GetSourceModel(1).ReductionSettings.PercentTriangles = 0.05f;
			StaticMesh->GetSourceModel(1).ReductionSettings.PercentVertices = 0.05f;
			StaticMesh->GetSourceModel(1).ReductionSettings.MaxNumOfTriangles = T / 200.f;
			StaticMesh->GetSourceModel(1).ReductionSettings.MaxNumOfVerts = V / 200.f;
			StaticMesh->GetSourceModel(1).ReductionSettings.TerminationCriterion = EStaticMeshReductionTerimationCriterion::Any;

			FMeshReductionSettings ReductionSettings1;
			ReductionSettings1.TerminationCriterion = EStaticMeshReductionTerimationCriterion::Any;
			//ReductionSettings1.MaxDeviation = 5.f;
			ReductionSettings1.PercentTriangles = 0.001f;
			ReductionSettings1.PercentVertices = 0.001f;
			ReductionSettings1.MaxNumOfTriangles = T / 1000.f;
			ReductionSettings1.MaxNumOfVerts = V / 1000.f;
			StaticMesh->GetSourceModel(2).ReductionSettings = ReductionSettings1;

			StaticMesh->Build();
			StaticMesh->MarkPackageDirty();
		}
	}
}
