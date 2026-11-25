#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "World/Destruction/OmnisStructuralDamageComponent.h"
#include "OmnisStructuralDamageSubsystem.generated.h"

class UOmnisStructuralDamageComponent;

UCLASS()
class OMNIS_API UOmnisStructuralDamageSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Omnis|Destruction")
    TArray<FOmnisStructuralDamageSnapshot> ExportSnapshots() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Destruction")
    void ImportSnapshots(const TArray<FOmnisStructuralDamageSnapshot>& Snapshots);

    void RegisterStructure(UOmnisStructuralDamageComponent* Component);
    void UnregisterStructure(UOmnisStructuralDamageComponent* Component);
    void RecordSnapshot(const FOmnisStructuralDamageSnapshot& Snapshot);

private:
    bool HasMutationAuthority() const;

    UPROPERTY()
    TMap<FName, FOmnisStructuralDamageSnapshot> StructureSnapshots;

    TMap<FName, TWeakObjectPtr<UOmnisStructuralDamageComponent>> ActiveStructures;
};
