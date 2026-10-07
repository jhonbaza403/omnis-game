#include "Online/Friends/OmnisPartySubsystem.h"

#include "Online/Safety/OmnisSafetySubsystem.h"

bool UOmnisPartySubsystem::CreateParty(FName OwnerId)
{
    if (OwnerId.IsNone() || PartyId != NAME_None || IsBlockedFromParty(OwnerId))
    {
        return false;
    }

    PartyId = FName(*FString::Printf(
        TEXT("Party.%s"),
        *FGuid::NewGuid().ToString(EGuidFormats::Digits)));

    Members.Reset();
    return AddMember(OwnerId, OwnerId.ToString());
}

bool UOmnisPartySubsystem::AddMember(FName UserId, FString DisplayName)
{
    if (UserId.IsNone() || IsBlockedFromParty(UserId) || Members.Num() >= 8)
    {
        return false;
    }

    if (Members.ContainsByPredicate([UserId](const FOmnisPartyMember& Member)
    {
        return Member.UserId == UserId;
    }))
    {
        return false;
    }

    DisplayName.TrimStartAndEndInline();
    if (DisplayName.IsEmpty())
    {
        DisplayName = UserId.ToString();
    }

    FOmnisPartyMember Member;
    Member.UserId = UserId;
    Member.DisplayName = DisplayName.Left(48);
    Members.Add(Member);
    return true;
}

bool UOmnisPartySubsystem::RemoveMember(FName UserId)
{
    const int32 Removed = Members.RemoveAll([UserId](const FOmnisPartyMember& Member)
    {
        return Member.UserId == UserId;
    });

    if (Members.Num() == 0)
    {
        PartyId = NAME_None;
    }

    return Removed > 0;
}

bool UOmnisPartySubsystem::SetMemberReady(FName UserId, bool bReady)
{
    for (FOmnisPartyMember& Member : Members)
    {
        if (Member.UserId == UserId)
        {
            Member.bReady = bReady;
            return true;
        }
    }

    return false;
}

bool UOmnisPartySubsystem::IsBlockedFromParty(FName UserId) const
{
    const UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        return false;
    }

    if (const UOmnisSafetySubsystem* Safety =
        GameInstance->GetSubsystem<UOmnisSafetySubsystem>())
    {
        return Safety->IsBlocked(UserId);
    }

    return false;
}
