#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FDOCEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;

    void GenerateStaticMeshLOD(FString& FolderPath);
};