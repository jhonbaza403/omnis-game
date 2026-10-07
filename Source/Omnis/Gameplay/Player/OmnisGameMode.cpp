#include "Gameplay/Player/OmnisGameMode.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Gameplay/Player/OmnisPlayerController.h"
#include "UI/HUD/OmnisHUD.h"
#include "Core/Cycle/OmnisCycleSubsystem.h"
#include "Core/Nexus/OmnisNexusSubsystem.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "World/Environment/OmnisWorldBootstrapSubsystem.h"
#include "World/Content/OmnisWorldContentSubsystem.h"

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
    if (!PlayerController) return;

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
    if (!PlayerController) return;

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
    }

    RestartPlayer(PlayerController);
}
