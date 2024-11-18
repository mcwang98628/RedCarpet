// Copyright Epic Games, Inc. All Rights Reserved.
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net.NetworkInformation;
using System.Text;
using System.Text.RegularExpressions;
using UnrealBuildTool;

public class AR51SDK : ModuleRules
{
    private UnrealTargetPlatform Platform;
    private UnrealTargetConfiguration Configuration;

    // name of root folders in the project folder
    private static readonly string GRPC_STRIPPED_FOLDER = "GrpcIncludes";
    private static readonly string GRPC_LIBS_FOLDER = "GrpcLibraries";

    private string INCLUDE_ROOT;
    private string LIB_ROOT;

    public class ModuleDepPaths
    {
        public readonly string[] HeaderPaths;
        public readonly string[] LibraryPaths;

        public ModuleDepPaths(string[] headerPaths, string[] libraryPaths)
        {
            HeaderPaths = headerPaths;
            LibraryPaths = libraryPaths;
        }

        public override string ToString()
        {
            return "Headers:\n" + string.Join("\n", HeaderPaths) + "\nLibs:\n" + string.Join("\n", LibraryPaths);
        }
    }

    [Conditional("DEBUG")]
    [Conditional("TRACE")]
    private void clog(params object[] objects)
    {
        Console.WriteLine(string.Join(", ", objects));
    }

    private IEnumerable<string> FindFilesInDirectory(string dir, string suffix = "")
    {
        List<string> matches = new List<string>();
        if (Directory.Exists(dir))
        {
            string[] files = Directory.GetFiles(dir);
            //Regex regex = new Regex(".+\\." + suffix);

            foreach (string file in files)
            {
                //Console.WriteLine("Checking file: " + file);
                matches.Add(file);
                //if (regex.Match(file).Success)
                //    matches.Add(file);
            }
        }

        return matches;
    }

    private string GetConfigurationString()
    {
        return (Configuration == UnrealTargetConfiguration.Shipping) ? "Release" : "Debug";
    }

    public ModuleDepPaths GatherDeps(ReadOnlyTargetRules Target)
    {
        string RootPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/"));

        INCLUDE_ROOT = Path.Combine(RootPath, GRPC_STRIPPED_FOLDER);
        LIB_ROOT = Path.Combine(RootPath, GRPC_LIBS_FOLDER);

        if (Target.Version.MajorVersion == 5)
        {
            LIB_ROOT = Path.Combine(LIB_ROOT, "Unreal5");
            INCLUDE_ROOT = Path.Combine(INCLUDE_ROOT, "Unreal5");
        }

        List<string> headers = new List<string>();
        List<string> libs = new List<string>();

        string PlatformLibRoot = "";

        if (Platform == UnrealTargetPlatform.Win64)
        {
            PlatformLibRoot = Path.Combine(LIB_ROOT, Platform.ToString());
            libs.AddRange(FindFilesInDirectory(PlatformLibRoot, "lib"));
        }
#if UE_5_2_OR_LATER
        else if (Platform == UnrealTargetPlatform.Android)
        {
            //if (Target.Architecture == UnrealArch.Arm64) // Check for ARM64 architecture
            //{
                PlatformLibRoot = Path.Combine(LIB_ROOT, "Android64");
                Console.WriteLine("Using Android Architcture 64 and folder: " + PlatformLibRoot);
            //}
            //else //if (Target.Architecture == UnrealArch.ARMV7) // Check for ARMv7 architecture
            //{
            //    PlatformLibRoot = Path.Combine(LIB_ROOT, "Android");
            //    Console.WriteLine("Using Android Architcture 7 and folder: " + PlatformLibRoot);
            //}
            libs.AddRange(FindFilesInDirectory(PlatformLibRoot, "a"));
        }
        else
        {
            Console.WriteLine("Cannot include GRPC library for unknown Platform: ", Platform.ToString());
        }
#else
        else{
            IAndroidToolChain ToolChain = AndroidExports.CreateToolChain(Target.ProjectFile);
            var Architectures = ToolChain.GetAllArchitectures();

             foreach (var arch in Architectures)
             {
                 Console.WriteLine("Architecture: " + arch);
             }

             if (Architectures.Contains("-arm64"))
             {
                 Console.WriteLine("64 is in Architectures");

                 PlatformLibRoot = Path.Combine(LIB_ROOT, "Android64");
             }
             else // (Architectures.Contains("-armv7" ))
             {
                 Console.WriteLine("7 is in Architectures");

                 PlatformLibRoot = Path.Combine(LIB_ROOT, "Android");
             }
             libs.AddRange(FindFilesInDirectory(PlatformLibRoot, "a"));
         }
#endif
        Console.WriteLine("Including GRPC libraries in directory: " + PlatformLibRoot);

        //clog("PlatformLibRoot: " + PlatformLibRoot + " number of libs: " + libs.ToArray().Length);
        //foreach(var l in libs)
        //{
        //    clog("lib: " + l);
        //}
        headers.Add(Path.Combine(INCLUDE_ROOT, "include"));
        headers.Add(Path.Combine(INCLUDE_ROOT, "third_party", "protobuf", "src"));

        return new ModuleDepPaths(headers.ToArray(), libs.ToArray());

    }

    public AR51SDK(ReadOnlyTargetRules Target) : base(Target)
    {
        //target allow us to build different configuration per OS (i.e. Windows/Android)
        PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI");
        PublicDefinitions.Add("GPR_FORBID_UNREACHABLE_CODE");
        PublicDefinitions.Add("GRPC_ALLOW_EXCEPTIONS=0");


        if (Target.Version.MajorVersion == 5)
        {
            PublicDefinitions.Add("UE_5_0_OR_LATER=1");
            if (Target.Version.MinorVersion >= 2)
                PublicDefinitions.Add("UE_5_2_OR_LATER=1");
            if (Target.Version.MinorVersion >= 3)
                PublicDefinitions.Add("UE_5_3_OR_LATER=1");
        }

        //TODO: We do this because in file generated_message_table_driven.h that located in protobuf sources 
        //TODO: line 174: static_assert(std::is_pod<AuxillaryParseTableField>::value, "");
        //TODO: causes С4647 level 3 warning __is_pod behavior change
        //TODO: UE4 threading some warnings as errors, and we have no chance to suppress this stuff
        //TODO: So, we don't want to change any third-party code, this why we add this definition
        PublicDefinitions.Add("__NVCC__");

        Platform = Target.Platform;
        Configuration = Target.Configuration;

        ModuleDepPaths moduleDepPaths = GatherDeps(Target);
        //Console.WriteLine(moduleDepPaths.ToString());

        PublicIncludePaths.AddRange(moduleDepPaths.HeaderPaths);
        PublicAdditionalLibraries.AddRange(moduleDepPaths.LibraryPaths);

        PublicDependencyModuleNames.AddRange(new string[] { "Core" });

        AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenSSL");
        AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");

        PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine" });

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Networking", "Sockets",
            "ProceduralMeshComponent", "AnimGraphRuntime"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "InputDevice" });
        string InfraworldRuntimePath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/InfraworldRuntime/"));

        PrivateIncludePaths.AddRange(
            new string[] {
                "AR51SDK/Private/Utilities",
                "AR51SDK/Private/Core",
                "AR51SDK/Private/Services",
                "AR51SDK/Private/Clients",
                "AR51SDK/Private/Clients/SkeletonClient",
                "ThirdParty/InfraworldRuntime/Private",
                "ThirdParty/InfraworldRuntime/Public",
                "AR51SDK/Private/protos"
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Services")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Clients")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Utilities")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Utilities/Heaps")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Utilities/Filters")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Adapters")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Adapters/HandsAdapter")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Adapters/HandsAdapter/Adapters")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Adapters/HandsAdapter/Data")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Public/Adapters/BoundaryAdapter")),
                Path.GetFullPath(Path.Combine(ModuleDirectory,"Private/protos")),
                Path.GetFullPath(Path.Combine(InfraworldRuntimePath,"Public"))
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );
    }
}
