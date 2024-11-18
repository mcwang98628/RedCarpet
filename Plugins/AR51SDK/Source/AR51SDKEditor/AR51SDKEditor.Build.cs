// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AR51SDKEditor : ModuleRules
{
    public AR51SDKEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "AnimationCore",
                "AnimGraph",
                "AnimGraphRuntime",
                "Core",
                "CoreUObject",
                "Engine",
            });

#if UE_5_0_OR_LATER
        PublicDependencyModuleNames.Add("AnimationWarpingRuntime");
#endif

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Projects",
                "InputCore",
                "UnrealEd",
                "LevelEditor",
                "CoreUObject",
                "Engine",
                "EngineSettings",
                "AndroidRuntimeSettings",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "Core",
                "AR51SDK",
            }
        );

        PrivateIncludePaths.AddRange(
                new string[] {
					"AR51SDKEditor/Private",
                    "AR51SDK/Private",
                });

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                        "BlueprintGraph",
                        "UnrealEd",
                }
            );

#if UE_5_0_OR_LATER
            PrivateDependencyModuleNames.Add("EditorFramework");
#endif
        }
        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Settings",
            }
            );
    }
}
