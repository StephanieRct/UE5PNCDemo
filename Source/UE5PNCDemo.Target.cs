// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UE5PNCDemoTarget : TargetRules
{
	public UE5PNCDemoTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("UE5PNCDemo"); 
    }
}
