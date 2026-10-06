#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisCharacterStateComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisCharacterStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisCharacterStateComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Narrative")
    void SetAlive(bool bNewAlive);

    UFUNCTION(BlueprintCallable, Category="Omnis|Narrative")
    void SetRelationship(FName SubjectId, int32 Value);

    UFUNCTION(BlueprintPure, Category="Omnis|Narrative")
    bool IsAlive() const { return bAlive; }

    UFUNCTION(BlueprintPure, Category="Omnis|Narrative")
    int32 GetRelationship(FName SubjectId) const;

private:
    UPROPERTY()
    bool bAlive = true;

    UPROPERTY()
    TMap<FName, int32> Relationships;
};
