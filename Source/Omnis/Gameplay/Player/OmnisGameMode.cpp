#include "Gameplay/Player/OmnisGameMode.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Gameplay/Player/OmnisPlayerController.h"
#include "Core/Cycle/OmnisCycleSubsystem.h"
#include "Core/Nexus/OmnisNexusSubsystem.h"
#include "Core/Save/OmnisSaveSubsystem.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "World/Environment/OmnisWorldBootstrapSubsystem.h"

AOmnisGameMode::AOmnisGameMode()
{
    DefaultPawnClass = AOmnisPlayerCharacter::StaticClass();
    PlayerControllerClass = AOmnisPlayerController::StaticClass();
}

void AOmnisGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisSaveSubsystem* Saves = GameInstance->GetSubsystem<UOmnisSaveSubsystem>())
        {
            if (!Saves->LoadFromSlot(1))
            {
                if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
                {
                    Cycle->BeginCycle();
                }
            }
        }
    }

    if (UOmnisWorldBootstrapSubsystem* Bootstrap = GetWorld()->GetSubsystem<UOmnisWorldBootstrapSubsystem>())
    {
        Bootstrap->BuildInitialWorld();
    }
}

void AOmnisGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisSaveSubsystem* Saves = GameInstance->GetSubsystem<UOmnisSaveSubsystem>())
        {
            Saves->SaveToSlot(1);
        }
    }

    Super::EndPlay(EndPlayReason);
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

        if (UOmnisSaveSubsystem* Saves = GameInstance->GetSubsystem<UOmnisSaveSubsystem>())
        {
            Saves->SaveToSlot(1);
        }
    }

    GetWorldTimerManager().SetTimer(
        RespawnTimer,
        FTimerDelegate::CreateUObject(this, &AOmnisGameMode::RespawnAfterNexus, PlayerController),
        1.5f,
        false
    );
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

        if (UOmnisSaveSubsystem* Saves = GameInstance->GetSubsystem<UOmnisSaveSubsystem>())
        {
            Saves->SaveToSlot(1);
        }
    }

    RestartPlayer(PlayerController);
}
