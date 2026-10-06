#include "Core/Subsystems/OmnisModuleRegistrySubsystem.h"

void UOmnisModuleRegistrySubsystem::RegisterModule(FName ModuleId)
{
    if (!ModuleId.IsNone())
    {
        RegisteredModules.Add(ModuleId);
    }
}

void UOmnisModuleRegistrySubsystem::UnregisterModule(FName ModuleId)
{
    RegisteredModules.Remove(ModuleId);
}

bool UOmnisModuleRegistrySubsystem::IsModuleRegistered(FName ModuleId) const
{
    return RegisteredModules.Contains(ModuleId);
}
