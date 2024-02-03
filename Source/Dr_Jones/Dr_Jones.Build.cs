// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Dr_Jones : ModuleRules
{
	public Dr_Jones(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine",
			"EnhancedInput",
			"InputCore", 
			"ProceduralMeshComponent", 
			"UMG", 
			"RHI",
			"Slate", 
			"SlateCore",
			"GeometryFramework",
			"GeometryCore",
			"zlib"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "AnimGraphRuntime" });

		PublicIncludePaths.AddRange(new string[] { "Dr_Jones" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
