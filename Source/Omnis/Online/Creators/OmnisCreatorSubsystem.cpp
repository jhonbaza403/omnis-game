#include "Online/Creators/OmnisCreatorSubsystem.h"

FString UOmnisCreatorSubsystem::CreateDraft(FString Title, FName Category)
{
    Title.TrimStartAndEndInline();
    if (Title.IsEmpty() || Category.IsNone())
    {
        return FString();
    }

    FOmnisCreation Creation;
    Creation.Id = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
    Creation.Title = Title.Left(80);
    Creation.Category = Category;
    Creations.Add(Creation.Id, Creation);
    return Creation.Id;
}

bool UOmnisCreatorSubsystem::AddElementToDraft(
    FString CreationId,
    FName ElementId)
{
    if (FOmnisCreation* Creation = Creations.Find(CreationId))
    {
        if (Creation->bPublished ||
            ElementId.IsNone() ||
            Creation->Elements.Contains(ElementId) ||
            Creation->Elements.Num() >= 64)
        {
            return false;
        }

        Creation->Elements.Add(ElementId);
        ++Creation->Version;
        return true;
    }

    return false;
}

bool UOmnisCreatorSubsystem::PublishCreation(FString CreationId)
{
    if (FOmnisCreation* Creation = Creations.Find(CreationId))
    {
        if (Creation->bPublished || Creation->Elements.Num() == 0)
        {
            return false;
        }

        Creation->bPublished = true;
        return true;
    }

    return false;
}

bool UOmnisCreatorSubsystem::LikeCreation(FString CreationId)
{
    if (FOmnisCreation* Creation = Creations.Find(CreationId))
    {
        if (!Creation->bPublished)
        {
            return false;
        }

        ++Creation->Likes;
        return true;
    }

    return false;
}

bool UOmnisCreatorSubsystem::GetCreation(
    FString CreationId,
    FOmnisCreation& OutCreation) const
{
    if (const FOmnisCreation* Creation = Creations.Find(CreationId))
    {
        OutCreation = *Creation;
        return true;
    }

    return false;
}

TArray<FOmnisCreation> UOmnisCreatorSubsystem::GetPublishedCreations() const
{
    TArray<FOmnisCreation> Result;
    for (const TPair<FString, FOmnisCreation>& Pair : Creations)
    {
        if (Pair.Value.bPublished)
        {
            Result.Add(Pair.Value);
        }
    }

    Result.Sort([](const FOmnisCreation& A, const FOmnisCreation& B)
    {
        return A.Likes > B.Likes;
    });

    return Result;
}
