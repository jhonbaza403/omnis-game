#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisModuleRegistrySubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisModuleRegistrySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Modules")
    void RegisterModule(FName ModuleId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Modules")
    void UnregisterModule(FName ModuleId);

    UFUNCTION(BlueprintPure, Category="Omnis|Modules")
    bool IsModuleRegistered(FName ModuleId) const;

private:
    UPROPERTY()
    TSet<FName> RegisteredModules;
};
