// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Labyrinthine : ModuleRules
{
    public Labyrinthine(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "Niagara" // Added for Niagara VFX support
        });

        // Add UMG so UWidgetComponent links correctly
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UMG"
        });

        // If you plan to create UMG widgets (Slate) in code, these are handy too:
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
