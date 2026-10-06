#include "Gameplay/Player/OmnisGameMode.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Gameplay/Player/OmnisPlayerController.h"
#include "Core/Cycle/OmnisCycleSubsystem.h"
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
        if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
        {
            Cycle->BeginCycle();
        }
    }

    if (UOmnisWorldBootstrapSubsystem* Bootstrap = GetWorld()->GetSubsystem<UOmnisWorldBootstrapSubsystem>())
    {
        Bootstrap->BuildInitialWorld();
    }
}
