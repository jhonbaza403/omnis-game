#include "Systems/Tags/OmnisTagLibrary.h"

bool UOmnisTagLibrary::HasAnyTag(const FGameplayTagContainer& Container, const FGameplayTagContainer& Query)
{
    return Container.HasAny(Query);
}

bool UOmnisTagLibrary::HasAllTags(const FGameplayTagContainer& Container, const FGameplayTagContainer& Query)
{
    return Container.HasAll(Query);
}
