#include "UI/HUD/OmnisHUD.h"

#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "UI/HUD/OmnisHUDWidget.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"

AOmnisHUD::AOmnisHUD()
{
    PrimaryActorTick.bCanEverTick = false;
}

FString AOmnisHUD::PhaseToText(uint8 PhaseValue)
{
    switch (PhaseValue)
    {
        case 0: return TEXT("AWAKENING");
        case 1: return TEXT("EXPLORATION");
        case 2: return TEXT("CRISIS");
        case 3: return TEXT("COLLAPSE");
        case 4: return TEXT("NEXUS");
        case 5: return TEXT("REBIRTH");
        default: return TEXT("UNKNOWN");
    }
}

void AOmnisHUD::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = GetOwningPlayerController())
    {
        HUDWidget = CreateWidget<UOmnisHUDWidget>(PC, UOmnisHUDWidget::StaticClass());
        if (HUDWidget)
        {
            HUDWidget->AddToViewport(50);
        }
    }
}

void AOmnisHUD::DrawHUD()
{
    Super::DrawHUD();

    // Retain the lightweight crosshair layer for aiming/interaction.
    if (!Canvas)
    {
        return;
    }

    APlayerController* PC = GetOwningPlayerController();
    if (!PC || !Cast<AOmnisPlayerCharacter>(PC->GetPawn()))
    {
        return;
    }

    const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

    FCanvasTileItem Horizontal(
        FVector2D(Center.X - 8.0f, Center.Y - 1.0f),
        FVector2D(16.0f, 2.0f),
        FLinearColor(0.7f, 0.9f, 1.0f, 0.8f)
    );
    Horizontal.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(Horizontal);

    FCanvasTileItem Vertical(
        FVector2D(Center.X - 1.0f, Center.Y - 8.0f),
        FVector2D(2.0f, 16.0f),
        FLinearColor(0.7f, 0.9f, 1.0f, 0.8f)
    );
    Vertical.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(Vertical);
}
