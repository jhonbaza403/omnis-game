#include "Gameplay/Player/OmnisPlayerController.h"

AOmnisPlayerController::AOmnisPlayerController()
{
    bShowMouseCursor = false;
}

void AOmnisPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameOnly());
}
