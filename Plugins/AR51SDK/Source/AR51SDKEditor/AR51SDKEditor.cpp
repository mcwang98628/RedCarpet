// Copyright Epic Games, Inc. All Rights Reserved.

#include "AR51SDKEditor.h"

#define LOCTEXT_NAMESPACE "FAR51SDKEditorModule"

void FAR51SDKEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FAR51SDKEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAR51SDKEditorModule, AR51SDKEditor)