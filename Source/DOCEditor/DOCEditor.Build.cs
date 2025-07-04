using UnrealBuildTool;

public class DOCEditor : ModuleRules
{
    public DOCEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "UnrealEd", "Slate", "SlateCore", "EditorStyle",
                "Kismet", "KismetCompiler", "AssetTools",
                "RenderCore", "InputCore",
                "MeshDescription",
                "MeshDescriptionOperations",
                "StaticMeshDescription",
                "MeshReductionInterface",
                "MeshUtilitiesCommon",
                "AssetRegistry",
                "MeshUtilities"
            });
        }

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "EnhancedInput",
            "UMG",
            "NavigationSystem"
        });

    }
}