// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TDShooter : ModuleRules
{
	public TDShooter(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "UMG", "Slate", "SlateCore" });

        PublicDependencyModuleNames.Add("OnlineSubsystem");
        PublicDependencyModuleNames.Add("OnlineSubsystemUtils");
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

    }
}
