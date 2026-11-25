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
            "AIModule",
            "InputCore",
            "GameplayTags",
            "UMG",
            "Slate",
            "SlateCore",
            "HTTP",
            "Json",
            "JsonUtilities"
        });
    }
}
