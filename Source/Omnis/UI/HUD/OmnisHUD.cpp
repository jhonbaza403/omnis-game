#include "UI/HUD/OmnisHUD.h"

#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Core/Cycle/OmnisCycleSubsystem.h"
#include "Core/Nexus/OmnisNexusSubsystem.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "GameFramework/PlayerController.h"

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

void AOmnisHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas)
    {
        return;
    }

    APlayerController* PC = GetOwningPlayerController();
    AOmnisPlayerCharacter* Player = PC ? Cast<AOmnisPlayerCharacter>(PC->GetPawn()) : nullptr;
    if (!Player)
    {
        return;
    }

    UFont* Font = GEngine ? GEngine->GetMediumFont() : nullptr;
    if (!Font)
    {
        return;
    }

    const float Padding = 32.0f;

    FCanvasTextItem TitleItem(
        FVector2D(Padding, Padding),
        FText::FromString(TEXT("OMNIS // EL ÚLTIMO CICLO")),
        Font,
        FLinearColor(0.72f, 0.92f, 1.0f, 1.0f)
    );
    TitleItem.Scale = FVector2D(1.15f, 1.15f);
    Canvas->DrawItem(TitleItem);

    int32 CycleIndex = 0;
    FString Phase = TEXT("BOOT");
    float WorldAge = 0.0f;

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UOmnisCycleSubsystem* Cycle = GI->GetSubsystem<UOmnisCycleSubsystem>())
        {
            const FOmnisCycleState State = Cycle->GetCycleState();
            CycleIndex = State.CycleIndex;
            Phase = PhaseToText(static_cast<uint8>(State.Phase));
            WorldAge = State.WorldAge;
        }
    }

    const float Health = Player->Health ? Player->Health->GetHealth() : 0.0f;
    const float MaxHealth = Player->Health ? Player->Health->GetMaxHealth() : 100.0f;

    FCanvasTextItem StatusItem(
        FVector2D(Padding, Padding + 34.0f),
        FText::FromString(FString::Printf(TEXT("CICLO %d   |   FASE %s   |   EDAD %0.1f"), CycleIndex, *Phase, WorldAge)),
        Font,
        FLinearColor::White
    );
    Canvas->DrawItem(StatusItem);

    FCanvasTextItem HealthItem(
        FVector2D(Padding, Canvas->ClipY - 94.0f),
        FText::FromString(FString::Printf(TEXT("VITALIDAD  %0.0f / %0.0f"), Health, MaxHealth)),
        Font,
        FLinearColor(0.65f, 1.0f, 0.78f, 1.0f)
    );
    Canvas->DrawItem(HealthItem);

    FCanvasTextItem ControlsItem(
        FVector2D(Padding, Canvas->ClipY - 58.0f),
        FText::FromString(TEXT("WASD MOVER   |   SHIFT CORRER   |   E INTERACTUAR   |   Q CONSTRUIR")),
        Font,
        FLinearColor(0.82f, 0.86f, 0.92f, 0.9f)
    );
    ControlsItem.Scale = FVector2D(0.8f, 0.8f);
    Canvas->DrawItem(ControlsItem);

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

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UOmnisNexusSubsystem* Nexus = GI->GetSubsystem<UOmnisNexusSubsystem>())
        {
            FCanvasTextItem NexusItem(
                FVector2D(Canvas->ClipX - 380.0f, Padding),
                FText::FromString(FString::Printf(
                    TEXT("NEXUS  %d PTOS   |   VITALIDAD +%d"),
                    Nexus->GetPermanentUpgradePoints(),
                    Nexus->GetPermanentVitalityLevel()
                )),
                Font,
                FLinearColor(0.86f, 0.72f, 1.0f, 1.0f)
            );
            NexusItem.Scale = FVector2D(0.78f, 0.78f);
            Canvas->DrawItem(NexusItem);
        }
    }
}
