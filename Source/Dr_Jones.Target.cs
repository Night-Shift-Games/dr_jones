// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Dr_JonesTarget : TargetRules
{
	public Dr_JonesTarget( TargetInfo Target) : base(Target)
	{
		Name = "Dr_Jones";
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "Dr_Jones" } );
	}
}
