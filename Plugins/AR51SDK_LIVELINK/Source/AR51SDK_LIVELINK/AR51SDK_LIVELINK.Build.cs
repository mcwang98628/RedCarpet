using UnrealBuildTool;

public class AR51SDK_LIVELINK : ModuleRules
{
	public AR51SDK_LIVELINK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
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
	            "ProceduralMeshComponent",
				"LiveLinkInterface",
                "LiveLink",
		        "AR51SDK",
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			});		
	}
}
