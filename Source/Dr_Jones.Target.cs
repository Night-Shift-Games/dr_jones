// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Dr_JonesTarget : TargetRules
{
	public Dr_JonesTarget( TargetInfo Target) : base(Target)
	{
		Name = "Dr_Jones";
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Dr_Jones" } );
	}
}
