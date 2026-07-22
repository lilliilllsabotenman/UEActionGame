// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpaceGameProject : ModuleRules
{
	public SpaceGameProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"CableComponent"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Json" });

		// StageGeneratorのデブリメッシュ列挙用(エディタ専用)
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("AssetRegistry");
		}

		PublicIncludePaths.AddRange(new string[] {
			"SpaceGameProject",
			"SpaceGameProject/Variant_Platforming",
			"SpaceGameProject/Variant_Platforming/Animation",
			"SpaceGameProject/Variant_Combat",
			"SpaceGameProject/Variant_Combat/AI",
			"SpaceGameProject/Variant_Combat/Animation",
			"SpaceGameProject/Variant_Combat/Gameplay",
			"SpaceGameProject/Variant_Combat/Interfaces",
			"SpaceGameProject/Variant_Combat/UI",
			"SpaceGameProject/Variant_SideScrolling",
			"SpaceGameProject/Variant_SideScrolling/AI",
			"SpaceGameProject/Variant_SideScrolling/Gameplay",
			"SpaceGameProject/Variant_SideScrolling/Interfaces",
			"SpaceGameProject/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
