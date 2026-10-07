#include "Gameplay/Player/OmnisGameMode.h"

#include "Core/Cycle/OmnisCycleSubsystem.h"
#include "Core/Nexus/OmnisNexusSubsystem.h"
#include "Dom/JsonObject.h"
#include "EngineUtils.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Gameplay/Player/OmnisPlayerController.h"
#include "Multiplayer/World/OmnisMultiplayerWorldSubsystem.h"
#include "Multiplayer/World/OmnisReplicatedWorldStateActor.h"
#include "Online/Services/OmnisOnlineServiceSubsystem.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "UI/HUD/OmnisHUD.h"
#include "World/Content/OmnisWorldContentSubsystem.h"
#include "World/Environment/OmnisWorldBootstrapSubsystem.h"

namespace
{
    FString BuildDeathPayload(AController* PlayerController)
    {
        const FString PlayerName = PlayerController && PlayerController->PlayerState
            ? PlayerController->PlayerState->GetPlayerName()
            : FString();

        const TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
        if (!PlayerName.IsEmpty())
        {
            Payload->SetStringField(TEXT("player_name"), PlayerName);
        }
        Payload->SetStringField(TEXT("cause"), TEXT("gameplay_death"));

        FString Output;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
        FJsonSerializer::Serialize(Payload, Writer);
        return Output;
    }
}

AOmnisGameMode::AOmnisGameMode()
{
    DefaultPawnClass = AOmnisPlayerCharacter::StaticClass();
    PlayerControllerClass = AOmnisPlayerController::StaticClass();
    HUDClass = AOmnisHUD::StaticClass();
}

void AOmnisGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
        {
            Cycle->BeginCycle();
        }
    }

    if (UOmnisWorldBootstrapSubsystem* Bootstrap = GetWorld()->GetSubsystem<UOmnisWorldBootstrapSubsystem>())
    {
        Bootstrap->BuildInitialWorld();
    }

    if (UOmnisWorldContentSubsystem* Content = GetWorld()->GetSubsystem<UOmnisWorldContentSubsystem>())
    {
        Content->BuildProductionWorld();
    }

    if (UOmnisMultiplayerWorldSubsystem* MultiplayerWorld =
        GetGameInstance() ? GetGameInstance()->GetSubsystem<UOmnisMultiplayerWorldSubsystem>() : nullptr)
    {
        MultiplayerWorld->RefreshWorldState();
    }

    if (HasAuthority() && GetWorld())
    {
        bool bHasWorldStateActor = false;
        for (TActorIterator<AOmnisReplicatedWorldStateActor> It(GetWorld()); It; ++It)
        {
            bHasWorldStateActor = true;
            break;
        }

        if (!bHasWorldStateActor)
        {
            GetWorld()->SpawnActor<AOmnisReplicatedWorldStateActor>(
                AOmnisReplicatedWorldStateActor::StaticClass(),
                FVector::ZeroVector,
                FRotator::ZeroRotator);
        }
    }
}

void AOmnisGameMode::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisNexusSubsystem* Nexus = GameInstance->GetSubsystem<UOmnisNexusSubsystem>())
        {
            if (AOmnisPlayerCharacter* Player = NewPlayer ? Cast<AOmnisPlayerCharacter>(NewPlayer->GetPawn()) : nullptr)
            {
                if (UOmnisHealthComponent* Health = Player->Health)
                {
                    const float MaxHealth = 100.0f + (10.0f * Nexus->GetPermanentVitalityLevel());
                    Health->SetMaxHealth(MaxHealth);
                    Health->Restore(MaxHealth);
                }
            }
        }
    }
}

void AOmnisGameMode::NotifyPlayerDeath(AController* PlayerController)
{
    if (!PlayerController)
    {
        return;
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
        {
            Cycle->EnterNexus();
        }

        if (UOmnisNexusSubsystem* Nexus = GameInstance->GetSubsystem<UOmnisNexusSubsystem>())
        {
            Nexus->AddPermanentUpgradePoints(1);
        }

        if (UOmnisOnlineServiceSubsystem* Online =
            GameInstance->GetSubsystem<UOmnisOnlineServiceSubsystem>())
        {
            Online->RecordWorldEvent(
                TEXT("player_death"),
                Online->GetPlayerId(),
                BuildDeathPayload(PlayerController),
                [](bool, int32, const FString&) {});

            Online->AdvanceWorldCycle(
                TEXT(""),
                TEXT("player_death"),
                [WeakGameMode = TWeakObjectPtr<AOmnisGameMode>(this)](
                    bool bSucceeded,
                    int32,
                    const FString&)
                {
                    if (bSucceeded && WeakGameMode.IsValid())
                    {
                        if (UOmnisMultiplayerWorldSubsystem* MultiplayerWorld =
                            WeakGameMode->GetGameInstance()
                                ? WeakGameMode->GetGameInstance()->GetSubsystem<UOmnisMultiplayerWorldSubsystem>()
                                : nullptr)
                        {
                            MultiplayerWorld->RefreshWorldState();
                        }

                        if (WeakGameMode->GetWorld())
                        {
                            for (TActorIterator<AOmnisReplicatedWorldStateActor> It(WeakGameMode->GetWorld()); It; ++It)
                            {
                                It->RefreshFromBackend();
                                break;
                            }
                        }
                    }
                });
        }
    }

    GetWorldTimerManager().SetTimer(
        RespawnTimer,
        FTimerDelegate::CreateUObject(this, &AOmnisGameMode::RespawnAfterNexus, PlayerController),
        1.5f,
        false);
}

void AOmnisGameMode::RespawnAfterNexus(AController* PlayerController)
{
    if (!PlayerController)
    {
        return;
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisNexusSubsystem* Nexus = GameInstance->GetSubsystem<UOmnisNexusSubsystem>())
        {
            Nexus->PurchaseVitalityUpgrade();
        }

        if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
        {
            Cycle->BeginRebirth();
        }

        if (UOmnisMultiplayerWorldSubsystem* MultiplayerWorld =
            GameInstance->GetSubsystem<UOmnisMultiplayerWorldSubsystem>())
        {
            MultiplayerWorld->RefreshWorldState();
        }
    }

    RestartPlayer(PlayerController);
}
