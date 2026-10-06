using UnrealBuildTool;

public class Omnis : ModuleRules
{
    public Omnis(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayTags",
            "Http",
            "Json",
            "JsonUtilities",
            "OnlineSubsystem"
        });
    }
}
