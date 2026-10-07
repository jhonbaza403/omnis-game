#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Core/Identity/OmnisAvatarSubsystem.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"
#include "Core/Progression/OmnisRewardSubsystem.h"
#include "Online/Competitive/OmnisRankedSubsystem.h"
#include "Online/Creators/OmnisCreatorSubsystem.h"
#include "Online/Friends/OmnisPartySubsystem.h"
#include "Online/Safety/OmnisSafetySubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FOmnisExperienceSystemsAutomation,
    "Omnis.Experience.Systems",
    EAutomationTestFlags::ApplicationContextMask |
    EAutomationTestFlags::SmokeFilter)

bool FOmnisExperienceSystemsAutomation::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Validating Omnis experience subsystems in a real Unreal runtime."));

    UOmnisAvatarSubsystem* Avatar = NewObject<UOmnisAvatarSubsystem>();
    TestNotNull(TEXT("Avatar subsystem"), Avatar);
    Avatar->InitializeStarterProfile();
    TestTrue(TEXT("Starter outfit unlocked"), Avatar->HasCosmetic(TEXT("Wanderer.Default")));

    UOmnisProgressionSubsystem* Progression = NewObject<UOmnisProgressionSubsystem>();
    TestNotNull(TEXT("Progression subsystem"), Progression);
    const int32 InitialXP = Progression->GetProgression().Experience;
    Progression->AddExperience(100);
    TestGreaterThan(TEXT("Experience increases"), Progression->GetProgression().Experience, InitialXP);

    UOmnisRewardSubsystem* Rewards = NewObject<UOmnisRewardSubsystem>();
    TestNotNull(TEXT("Reward subsystem"), Rewards);
    const FOmnisRewardGrant Reward =
        Rewards->GrantActivityReward(TEXT("Automation"), 25, TEXT("Emote.Automation"));
    TestEqual(TEXT("Reward activity"), Reward.ActivityId, FName(TEXT("Automation")));
    TestEqual(TEXT("Reward XP"), Reward.Experience, 25);
    TestEqual(TEXT("Reward count"), Rewards->GetRewardCount(), 1);

    UOmnisRankedSubsystem* Ranked = NewObject<UOmnisRankedSubsystem>();
    TestNotNull(TEXT("Ranked subsystem"), Ranked);
    const int32 InitialRating = Ranked->GetRankedState().Rating;
    Ranked->RecordMatchResult(true);
    TestGreaterThan(TEXT("Winning increases rating"), Ranked->GetRankedState().Rating, InitialRating);
    Ranked->RecordMatchResult(false);
    TestEqual(TEXT("Two ranked matches recorded"), Ranked->GetRankedState().Wins + Ranked->GetRankedState().Losses, 2);

    UOmnisCreatorSubsystem* Creators = NewObject<UOmnisCreatorSubsystem>();
    TestNotNull(TEXT("Creator subsystem"), Creators);
    const FString CreationId = Creators->CreateDraft(TEXT("Automation Plaza"), TEXT("Community"));
    TestFalse(TEXT("Creation id is valid"), CreationId.IsEmpty());
    TestTrue(TEXT("Element added"), Creators->AddElementToDraft(CreationId, TEXT("Building.Block")));
    TestTrue(TEXT("Creation published"), Creators->PublishCreation(CreationId));
    TestTrue(TEXT("Published creation can receive likes"), Creators->LikeCreation(CreationId));

    UOmnisSafetySubsystem* Safety = NewObject<UOmnisSafetySubsystem>();
    TestNotNull(TEXT("Safety subsystem"), Safety);
    TestTrue(TEXT("Player can initially interact"), Safety->CanInteract(TEXT("player-a")));
    TestTrue(TEXT("Player can be blocked"), Safety->BlockPlayer(TEXT("player-a")));
    TestFalse(TEXT("Blocked player cannot interact"), Safety->CanInteract(TEXT("player-a")));
    TestTrue(
        TEXT("Player can be reported"),
        Safety->ReportPlayer(
            TEXT("player-b"),
            TEXT("player-a"),
            EOmnisSafetyReason::Spam,
            TEXT("Automation report")));

    UOmnisPartySubsystem* Party = NewObject<UOmnisPartySubsystem>();
    TestNotNull(TEXT("Party subsystem"), Party);
    TestTrue(TEXT("Party can be created"), Party->CreateParty(TEXT("player-b")));
    TestTrue(TEXT("Party member can be added"), Party->AddMember(TEXT("player-c"), TEXT("Explorer")));
    TestTrue(TEXT("Party member can become ready"), Party->SetMemberReady(TEXT("player-c"), true));
    TestEqual(TEXT("Party member count"), Party->GetMembers().Num(), 2);

    return true;
}

#endif
