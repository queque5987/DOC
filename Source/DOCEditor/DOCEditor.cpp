#include "DOCEditor.h"
#include "Modules/ModuleManager.h"

class FDOCEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("DOCEditor module Started"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("DOCEditor module Ended"));
    }
};

IMPLEMENT_MODULE(FDOCEditorModule, DOCEditor)