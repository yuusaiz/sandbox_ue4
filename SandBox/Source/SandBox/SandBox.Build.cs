// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SandBox : ModuleRules
{
	public SandBox(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTags", "Slate", "SlateCore", "WebBrowser", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        // ���L��ǉ�
        string OpenCVPath = Path.Combine(EngineDirectory, "Plugins\\Compositing\\OpenCVLensDistortion\\Source\\ThirdParty\\OpenCV");
        PublicSystemIncludePaths.Add(Path.Combine(OpenCVPath, "include"));
        //string OpenGLLibPath = "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.14393.0\\um\\x64";
        string OpenCVLibPath = Path.Combine(EngineDirectory, "Plugins\\Compositing\\OpenCVLensDistortion\\Source\\ThirdParty\\OpenCV\\lib");
        //PublicSystemLibraryPaths.Add(Path.Combine(OpenCVLibPath, "Win64"));

        PublicLibraryPaths.Add(Path.Combine(OpenCVLibPath, "Win64"));
        PublicAdditionalLibraries.Add("opencv_world331.lib");
        PublicAdditionalLibraries.Add("opengl32.lib");
    }
    // �ǉ�
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
    }
    //PluginDirectory, EngineDirectory �͂��̂܂܎g����iSource/Programs/UnrealBuildTool/Configuration/ModuleRules.cs�j

}
