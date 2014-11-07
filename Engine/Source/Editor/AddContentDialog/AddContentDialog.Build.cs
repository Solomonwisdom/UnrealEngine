// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AddContentDialog : ModuleRules
{
	public AddContentDialog(TargetInfo Target)
	{
		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"AssetTools",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"Slate",
				"SlateCore",
				"InputCore",
				"Json",
				"EditorStyle",
				"DirectoryWatcher",
				"DesktopPlatform",
				"PakFile",
				"ImageWrapper",
				"UnrealEd"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
			}
		);
		
		PublicIncludePathModuleNames.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"Editor/AddContentDialog/Private",
				"Editor/AddContentDialog/Private/ViewModels",
				"Editor/AddContentDialog/Private/ContentSourceProviders/AssetPack",
				"Editor/AddContentDialog/Private/ContentSourceProviders/FeaturePack",
			}
		);
	}
}
