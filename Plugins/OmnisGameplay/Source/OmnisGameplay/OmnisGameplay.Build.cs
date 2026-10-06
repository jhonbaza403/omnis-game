using UnrealBuildTool;

public class OmnisGameplay : ModuleRules
{
    public OmnisGameplay(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.Add("Core");
    }
}
