#include "Systems/Achievements/OmnisAchievementSubsystem.h"

#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Platform/OmnisPlatformSubsystem.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UOmnisAchievementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadDefinitions();
}

bool UOmnisAchievementSubsystem::LoadDefinitions()
{
    Definitions.Reset();

    const FString Path = FPaths::Combine(
        FPaths::ProjectContentDir(),
        TEXT("Omnis/Data/catalog.json")
    );

    FString Json;
    if (!FFileHelper::LoadFileToString(Json, *Path))
    {
        return false;
    }

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    if (!Root->TryGetArrayField(TEXT("achievements"), Values) || !Values)
    {
        return false;
    }

    for (const TSharedPtr<FJsonValue>& Value : *Values)
    {
        const TSharedPtr<FJsonObject> Object = Value->AsObject();
        if (!Object.IsValid())
        {
            continue;
        }

        FOmnisAchievementDefinition Definition;
        Object->TryGetStringField(TEXT("id"), Definition.Id);
        Object->TryGetStringField(TEXT("name"), Definition.Name);
        Object->TryGetStringField(TEXT("description"), Definition.Description);

        if (!Definition.Id.IsEmpty() && !Definition.Name.IsEmpty() && !Definition.Description.IsEmpty())
        {
            Definitions.Add(MoveTemp(Definition));
        }
    }

    return Definitions.Num() > 0;
}

bool UOmnisAchievementSubsystem::ReportProgress(const FString& AchievementId, float ProgressPercent)
{
    if (!HasDefinition(AchievementId))
    {
        return false;
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisPlatformSubsystem* Platform = GameInstance->GetSubsystem<UOmnisPlatformSubsystem>())
        {
            return Platform->UnlockAchievement(AchievementId, FMath::Clamp(ProgressPercent, 0.0f, 100.0f));
        }
    }

    return false;
}

bool UOmnisAchievementSubsystem::Unlock(const FString& AchievementId)
{
    return ReportProgress(AchievementId, 100.0f);
}

bool UOmnisAchievementSubsystem::HasDefinition(const FString& AchievementId) const
{
    return Definitions.ContainsByPredicate([&AchievementId](const FOmnisAchievementDefinition& Definition)
    {
        return Definition.Id == AchievementId;
    });
}
