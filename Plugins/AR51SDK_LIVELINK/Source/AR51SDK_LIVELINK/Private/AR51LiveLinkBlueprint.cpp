// Copyright Epic Games, Inc. All Rights Reserved.

#include "AR51LiveLinkBlueprint.h"
#include "Interfaces/IPluginManager.h"


#define LOCTEXT_NAMESPACE "FAR51LiveLinkBlueprintModule"

void FAR51LiveLinkBlueprintModule::StartupModule()
{
    TSharedPtr<IPlugin> AR51SDKPlugin = IPluginManager::Get().FindPlugin("AR51SDK");

    if (AR51SDKPlugin.IsValid() && !AR51SDKPlugin->IsEnabled())
    {        
        // Enable the plugin as necessary
        // Note: Unreal doesn't provide a direct method to enable a plugin at runtime. 
        // Enabling/disabling plugins is usually done via the project settings or the .uplugin file.
        UE_LOG(LogTemp, Warning, TEXT("AR51SDK plugin is not enabled. Please enable it."));
    }

    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}
void FAR51LiveLinkBlueprintModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAR51LiveLinkBlueprintModule, AR51LiveLinkBlueprint)