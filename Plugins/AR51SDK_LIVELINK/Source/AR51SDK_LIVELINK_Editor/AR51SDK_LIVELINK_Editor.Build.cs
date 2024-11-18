using UnrealBuildTool;


public class AR51SDK_LIVELINK_Editor : ModuleRules
{
    public AR51SDK_LIVELINK_Editor(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Projects",
                "Networking",
                "Sockets",
                "AnimationCore",
                "AnimGraph",
                "AnimGraphRuntime",

                "ProceduralMeshComponent",
                "AR51SDK_LIVELINK",
                "LiveLinkInterface",
                "LiveLink",
                "AR51SDK",
            });


        // Define UE_5_0_OR_LATER based on engine version
        if (Target.Version.MajorVersion >= 5)
        {
            PublicDefinitions.Add("UE_5_0_OR_LATER=1");
            PublicDependencyModuleNames.Add("AnimationWarpingRuntime");
        }
        else
        {
            PublicDefinitions.Add("UE_5_0_OR_LATER=0");
        }

        //#if UE_5_0_OR_LATER
        //        PublicDependencyModuleNames.Add("AnimationWarpingRuntime");
        //#endif

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
                "AR51SDK_LIVELINK",                 
            });

        PrivateIncludePaths.AddRange(
                new string[] {
                    "AR51SDK_LIVELINK_Editor/Private",
                    "AR51SDK_LIVELINK/Private",
                });

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "BlueprintGraph",
                    "UnrealEd",
                });

            if (Target.Version.MajorVersion >= 5)
            {
                PrivateDependencyModuleNames.Add("EditorFramework");
            }
        }
        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Settings",
            });
    }
}
