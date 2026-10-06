using UnrealBuildTool;

public class OmnisNarrative : ModuleRules
{
    public OmnisNarrative(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.Add("Core");
    }
}
