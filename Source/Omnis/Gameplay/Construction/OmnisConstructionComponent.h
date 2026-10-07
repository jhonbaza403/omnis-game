#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisConstructionComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisConstructionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisConstructionComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Construction")
    AActor* ConstructForward();

    UFUNCTION(Server, Reliable)
    void ServerConstructForward();

    UFUNCTION(BlueprintCallable, Category="Omnis|Construction")
    bool RemoveLastPiece();

    UFUNCTION(BlueprintPure, Category="Omnis|Construction")
    int32 GetBuiltPieceCount() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Construction")
    float BuildDistance = 220.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Construction")
    float GridSize = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Construction")
    int32 MaxPieces = 128;

private:
    FVector SnapLocation(const FVector& Location) const;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> BuiltPieces;
};
