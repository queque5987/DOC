// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DOC : ModuleRules
{
	public DOC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "UMG", "NavigationSystem", "Niagara", "AIModule", "GameplayTasks", "Kismet", "HTTP", "Json", "NNECore", "NeuralNetworkInference" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});
        PublicIncludePaths.Add("DOC");
    }
}
