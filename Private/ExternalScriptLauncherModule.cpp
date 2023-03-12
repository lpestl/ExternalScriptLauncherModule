// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExternalScriptLauncherModule/Public/ExternalScriptLauncherModule.h"

#define LOCTEXT_NAMESPACE "ExternalScriptLauncherModule"

void FExternalScriptLauncherModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; 
}

void FExternalScriptLauncherModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExternalScriptLauncherModule, ExternalScriptLauncherModule)