#include "Gameplay/Player/OmnisPlayerController.h"

#include "Online/Authentication/OmnisAuthenticationSubsystem.h"
#include "UI/HUD/OmnisHUDStateSubsystem.h"

AOmnisPlayerController::AOmnisPlayerController()
{
    bShowMouseCursor = false;
}

void AOmnisPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameOnly());

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UOmnisAuthenticationSubsystem* Auth =
            GameInstance->GetSubsystem<UOmnisAuthenticationSubsystem>();
        UOmnisHUDStateSubsystem* HUD =
            GameInstance->GetSubsystem<UOmnisHUDStateSubsystem>();

        if (Auth && HUD)
        {
            Auth->RefreshPlatformIdentity();
            const FString DisplayName = Auth->GetDisplayName();
            if (!DisplayName.IsEmpty())
            {
                HUD->SetActiveUserDisplayName(DisplayName);
            }
        }
    }
}
