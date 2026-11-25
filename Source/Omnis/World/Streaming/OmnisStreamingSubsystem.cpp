#include "World/Streaming/OmnisStreamingSubsystem.h"

#include "Components/SceneComponent.h"
#include "Components/WorldPartitionStreamingSourceComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "WorldPartition/WorldPartitionStreamingSource.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

void UOmnisStreamingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bCatalogLoaded = LoadRegionAnchorCatalog();

    if (!bCatalogLoaded)
    {
        UE_LOG(LogTemp, Error,
            TEXT("OMNIS World Partition streaming is unavailable: Venezuela region anchor catalog could not be loaded."));
    }
}

void UOmnisStreamingSubsystem::Deinitialize()
{
    ReleaseAllStreamingSources();
    RegionAnchors.Reset();
    bCatalogLoaded = false;
    Super::Deinitialize();
}

bool UOmnisStreamingSubsystem::LoadRegionAnchorCatalog()
{
    RegionAnchors.Reset();

    const FString CatalogPath = FPaths::Combine(
        FPaths::ProjectContentDir(),
        TEXT("Omnis/Data/venezuela-triangle-v1.json"));

    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *CatalogPath))
    {
        UE_LOG(LogTemp, Error, TEXT("OMNIS region catalog missing: %s"), *CatalogPath);
        return false;
    }

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("OMNIS region catalog contains invalid JSON."));
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Regions = nullptr;
    if (!Root->TryGetArrayField(TEXT("regions"), Regions) || !Regions)
    {
        UE_LOG(LogTemp, Error, TEXT("OMNIS region catalog has no regions array."));
        return false;
    }

    for (const TSharedPtr<FJsonValue>& RegionValue : *Regions)
    {
        const TSharedPtr<FJsonObject> Region = RegionValue.IsValid()
            ? RegionValue->AsObject()
            : nullptr;
        if (!Region.IsValid())
        {
            continue;
        }

        FString RegionId;
        const TArray<TSharedPtr<FJsonValue>>* Subregions = nullptr;
        if (!Region->TryGetStringField(TEXT("id"), RegionId) ||
            RegionId.IsEmpty() ||
            !Region->TryGetArrayField(TEXT("subregions"), Subregions) ||
            !Subregions)
        {
            continue;
        }

        TArray<FVector> Anchors;
        for (const TSharedPtr<FJsonValue>& SubregionValue : *Subregions)
        {
            const TSharedPtr<FJsonObject> Subregion = SubregionValue.IsValid()
                ? SubregionValue->AsObject()
                : nullptr;
            if (!Subregion.IsValid())
            {
                continue;
            }

            const TArray<TSharedPtr<FJsonValue>>* AnchorValues = nullptr;
            if (!Subregion->TryGetArrayField(TEXT("anchor"), AnchorValues) ||
                !AnchorValues ||
                AnchorValues->Num() != 3 ||
                !(*AnchorValues)[0].IsValid() ||
                !(*AnchorValues)[1].IsValid() ||
                !(*AnchorValues)[2].IsValid())
            {
                continue;
            }

            const double X = (*AnchorValues)[0]->AsNumber();
            const double Y = (*AnchorValues)[1]->AsNumber();
            const double Z = (*AnchorValues)[2]->AsNumber();

            if (!FMath::IsFinite(X) || !FMath::IsFinite(Y) || !FMath::IsFinite(Z))
            {
                continue;
            }

            Anchors.Add(FVector(X, Y, Z));
        }

        if (!Anchors.IsEmpty())
        {
            RegionAnchors.Add(FName(*RegionId), MoveTemp(Anchors));
        }
    }

    return RegionAnchors.Num() > 0;
}

void UOmnisStreamingSubsystem::RequestRegion(FName RegionId)
{
    if (RegionId.IsNone() || !bCatalogLoaded || !GetWorld())
    {
        return;
    }

    if (HasValidRegionSources(RegionId))
    {
        return;
    }

    // Remove stale providers before attempting to rebuild this region request.
    ReleaseRegion(RegionId);

    const TArray<FVector>* Anchors = RegionAnchors.Find(RegionId);
    if (!Anchors || Anchors->IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("OMNIS streaming request rejected: region '%s' has no catalogued subregion anchors."),
            *RegionId.ToString());
        return;
    }

    UWorld* World = GetWorld();
    UWorldPartitionSubsystem* WorldPartitionSubsystem =
        World ? World->GetSubsystem<UWorldPartitionSubsystem>() : nullptr;
    if (!WorldPartitionSubsystem)
    {
        UE_LOG(LogTemp, Error,
            TEXT("OMNIS streaming request rejected for '%s': this world has no World Partition subsystem."),
            *RegionId.ToString());
        return;
    }

    TArray<TObjectPtr<AActor>> CreatedActors;
    CreatedActors.Reserve(Anchors->Num());

    for (int32 Index = 0; Index < Anchors->Num(); ++Index)
    {
        const FVector& Anchor = (*Anchors)[Index];
        const FString SafeRegionName = RegionId.ToString().Replace(TEXT("."), TEXT("_"));

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Name = MakeUniqueObjectName(
            World,
            AActor::StaticClass(),
            FName(*FString::Printf(
                TEXT("OMNIS_WPSource_%s_%02d"),
                *SafeRegionName,
                Index)));
        SpawnParameters.ObjectFlags |= RF_Transient;
        SpawnParameters.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* SourceOwner = World->SpawnActor<AActor>(
            AActor::StaticClass(),
            FTransform(FRotator::ZeroRotator, Anchor),
            SpawnParameters);

        if (!SourceOwner)
        {
            UE_LOG(LogTemp, Error,
                TEXT("Failed to create World Partition streaming source owner for '%s' anchor %d."),
                *RegionId.ToString(),
                Index);
            for (AActor* CreatedActor : CreatedActors)
            {
                if (IsValid(CreatedActor))
                {
                    if (UWorldPartitionStreamingSourceComponent* ExistingSource =
                        CreatedActor->FindComponentByClass<UWorldPartitionStreamingSourceComponent>())
                    {
                        ExistingSource->DisableStreamingSource();
                    }
                    CreatedActor->Destroy();
                }
            }
            return;
        }

        SourceOwner->SetReplicates(false);
        SourceOwner->SetActorEnableCollision(false);
        SourceOwner->SetActorTickEnabled(false);

        USceneComponent* Root = NewObject<USceneComponent>(
            SourceOwner,
            TEXT("OMNISStreamingSourceRoot"));
        if (!Root)
        {
            SourceOwner->Destroy();
            for (AActor* CreatedActor : CreatedActors)
            {
                if (IsValid(CreatedActor))
                {
                    if (UWorldPartitionStreamingSourceComponent* ExistingSource =
                        CreatedActor->FindComponentByClass<UWorldPartitionStreamingSourceComponent>())
                    {
                        ExistingSource->DisableStreamingSource();
                    }
                    CreatedActor->Destroy();
                }
            }
            return;
        }

        Root->SetMobility(EComponentMobility::Movable);
        SourceOwner->SetRootComponent(Root);
        Root->RegisterComponent();
        SourceOwner->SetActorLocation(Anchor);

        UWorldPartitionStreamingSourceComponent* StreamingSource =
            NewObject<UWorldPartitionStreamingSourceComponent>(
                SourceOwner,
                TEXT("OMNISWorldPartitionStreamingSource"));
        if (!StreamingSource)
        {
            SourceOwner->Destroy();
            for (AActor* CreatedActor : CreatedActors)
            {
                if (IsValid(CreatedActor))
                {
                    if (UWorldPartitionStreamingSourceComponent* ExistingSource =
                        CreatedActor->FindComponentByClass<UWorldPartitionStreamingSourceComponent>())
                    {
                        ExistingSource->DisableStreamingSource();
                    }
                    CreatedActor->Destroy();
                }
            }
            return;
        }

        SourceOwner->AddInstanceComponent(StreamingSource);
        StreamingSource->TargetState = EStreamingSourceTargetState::Activated;
        StreamingSource->Priority = EStreamingSourcePriority::Normal;
        StreamingSource->SetIsReplicated(false);
        StreamingSource->RegisterComponent();
        StreamingSource->EnableStreamingSource();

        if (!WorldPartitionSubsystem->IsStreamingSourceProviderRegistered(StreamingSource))
        {
            UE_LOG(LogTemp, Error,
                TEXT("World Partition did not register streaming source for region '%s', anchor %d."),
                *RegionId.ToString(),
                Index);
            StreamingSource->DisableStreamingSource();
            SourceOwner->Destroy();
            for (AActor* CreatedActor : CreatedActors)
            {
                if (IsValid(CreatedActor))
                {
                    if (UWorldPartitionStreamingSourceComponent* ExistingSource =
                        CreatedActor->FindComponentByClass<UWorldPartitionStreamingSourceComponent>())
                    {
                        ExistingSource->DisableStreamingSource();
                    }
                    CreatedActor->Destroy();
                }
            }
            return;
        }

        CreatedActors.Add(SourceOwner);
    }

    if (!CreatedActors.IsEmpty())
    {
        RegionStreamingSourceActors.Add(RegionId, MoveTemp(CreatedActors));
    }
}

void UOmnisStreamingSubsystem::ReleaseRegion(FName RegionId)
{
    if (RegionId.IsNone())
    {
        return;
    }

    TArray<TObjectPtr<AActor>> SourceActors;
    if (RegionStreamingSourceActors.RemoveAndCopyValue(RegionId, SourceActors))
    {
        for (AActor* SourceOwner : SourceActors)
        {
            if (!IsValid(SourceOwner))
            {
                continue;
            }

            if (UWorldPartitionStreamingSourceComponent* StreamingSource =
                SourceOwner->FindComponentByClass<UWorldPartitionStreamingSourceComponent>())
            {
                StreamingSource->DisableStreamingSource();
            }

            SourceOwner->Destroy();
        }
    }
}

bool UOmnisStreamingSubsystem::IsRegionStreamingComplete(FName RegionId) const
{
    const TArray<TObjectPtr<AActor>>* SourceActors =
        RegionStreamingSourceActors.Find(RegionId);
    UWorld* World = GetWorld();
    UWorldPartitionSubsystem* WorldPartitionSubsystem =
        World ? World->GetSubsystem<UWorldPartitionSubsystem>() : nullptr;

    if (!SourceActors || SourceActors->IsEmpty() || !WorldPartitionSubsystem)
    {
        return false;
    }

    for (AActor* SourceOwner : *SourceActors)
    {
        if (!IsValid(SourceOwner))
        {
            return false;
        }

        UWorldPartitionStreamingSourceComponent* StreamingSource =
            SourceOwner->FindComponentByClass<UWorldPartitionStreamingSourceComponent>();
        if (!StreamingSource ||
            !StreamingSource->IsStreamingSourceEnabled() ||
            !WorldPartitionSubsystem->IsStreamingSourceProviderRegistered(StreamingSource) ||
            !WorldPartitionSubsystem->IsStreamingCompleted(StreamingSource))
        {
            return false;
        }
    }

    return true;
}

int32 UOmnisStreamingSubsystem::GetRegionStreamingSourceCount(FName RegionId) const
{
    const TArray<TObjectPtr<AActor>>* SourceActors =
        RegionStreamingSourceActors.Find(RegionId);
    return SourceActors ? SourceActors->Num() : 0;
}

bool UOmnisStreamingSubsystem::HasValidRegionSources(FName RegionId) const
{
    const TArray<TObjectPtr<AActor>>* SourceActors =
        RegionStreamingSourceActors.Find(RegionId);
    UWorld* World = GetWorld();
    UWorldPartitionSubsystem* WorldPartitionSubsystem =
        World ? World->GetSubsystem<UWorldPartitionSubsystem>() : nullptr;

    if (!SourceActors || SourceActors->IsEmpty() || !WorldPartitionSubsystem)
    {
        return false;
    }

    for (AActor* SourceOwner : *SourceActors)
    {
        if (!IsValid(SourceOwner))
        {
            return false;
        }

        UWorldPartitionStreamingSourceComponent* StreamingSource =
            SourceOwner->FindComponentByClass<UWorldPartitionStreamingSourceComponent>();
        if (!StreamingSource ||
            !StreamingSource->IsStreamingSourceEnabled() ||
            !WorldPartitionSubsystem->IsStreamingSourceProviderRegistered(StreamingSource))
        {
            return false;
        }
    }

    return true;
}

void UOmnisStreamingSubsystem::ReleaseAllStreamingSources()
{
    TArray<FName> RegionIds;
    RegionStreamingSourceActors.GetKeys(RegionIds);
    for (const FName RegionId : RegionIds)
    {
        ReleaseRegion(RegionId);
    }
}
