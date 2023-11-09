// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Dr_JonesEditorTarget : TargetRules
{
	public Dr_JonesEditorTarget( TargetInfo Target) : base(Target)
	{
		Name = "Dr_JonesEditor";
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Dr_Jones", "DrJonesEditor" } );
	}
}
