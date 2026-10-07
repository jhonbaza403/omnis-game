#include "UI/HUD/OmnisHUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/Cycle/OmnisCycleSubsystem.h"
#include "Core/Nexus/OmnisNexusSubsystem.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"
#include "Engine/Engine.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Online/Competitive/OmnisRankedSubsystem.h"
#include "Online/Friends/OmnisPartySubsystem.h"
#include "Systems/Damage/OmnisHealthComponent.h"

UOmnisHUDWidget::UOmnisHUDWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UOmnisHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BuildLayout();
    RefreshHUD();
}

void UOmnisHUDWidget::NativeTick(
    const FGeometry& MyGeometry,
    float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    RefreshAccumulator += InDeltaTime;
    if (RefreshAccumulator >= 0.25f)
    {
        RefreshAccumulator = 0.0f;
        RefreshHUD();
    }
}

void UOmnisHUDWidget::BuildLayout()
{
    if (WidgetTree->RootWidget)
    {
        return;
    }

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
        UCanvasPanel::StaticClass(), TEXT("Root"));
    WidgetTree->RootWidget = Root;

    TitleText = WidgetTree->ConstructWidget<UTextBlock>(
        UTextBlock::StaticClass(), TEXT("Title"));
    TitleText->SetText(FText::FromString(TEXT("OMNIS // EL ÚLTIMO CICLO")));
    Root->AddChild(TitleText);
    if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(TitleText->Slot))
    {
        Slot->SetPosition(FVector2D(32,24));
        Slot->SetAutoSize(true);
    }

    WorldText = WidgetTree->ConstructWidget<UTextBlock>(
        UTextBlock::StaticClass(), TEXT("World"));
    Root->AddChild(WorldText);
    if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(WorldText->Slot))
    {
        Slot->SetPosition(FVector2D(32,58));
        Slot->SetAutoSize(true);
    }

    ProgressionText = WidgetTree->ConstructWidget<UTextBlock>(
        UTextBlock::StaticClass(), TEXT("Progression"));
    Root->AddChild(ProgressionText);
    if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(ProgressionText->Slot))
    {
        Slot->SetPosition(FVector2D(32,92));
        Slot->SetAutoSize(true);
    }

    SocialText = WidgetTree->ConstructWidget<UTextBlock>(
        UTextBlock::StaticClass(), TEXT("Social"));
    Root->AddChild(SocialText);
    if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(SocialText->Slot))
    {
        Slot->SetPosition(FVector2D(32,126));
        Slot->SetAutoSize(true);
    }

    HealthText = WidgetTree->ConstructWidget<UTextBlock>(
        UTextBlock::StaticClass(), TEXT("Health"));
    Root->AddChild(HealthText);
    if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(HealthText->Slot))
    {
        Slot->SetAnchors(FAnchors(0,1,0,1));
        Slot->SetAlignment(FVector2D(0,1));
        Slot->SetPosition(FVector2D(32,-92));
        Slot->SetAutoSize(true);
    }

    HealthBar = WidgetTree->ConstructWidget<UProgressBar>(
        UProgressBar::StaticClass(), TEXT("HealthBar"));
    Root->AddChild(HealthBar);
    if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(HealthBar->Slot))
    {
        Slot->SetAnchors(FAnchors(0,1,0,1));
        Slot->SetAlignment(FVector2D(0,1));
        Slot->SetPosition(FVector2D(32,-58));
        Slot->SetSize(FVector2D(320,12));
    }

    ControlsText = WidgetTree->ConstructWidget<UTextBlock>(
        UTextBlock::StaticClass(), TEXT("Controls"));
    ControlsText->SetText(FText::FromString(
        TEXT("WASD MOVER | SHIFT CORRER | E INTERACTUAR | Q CONSTRUIR")));
    Root->AddChild(ControlsText);
    if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(ControlsText->Slot))
    {
        Slot->SetAnchors(FAnchors(0,1,0,1));
        Slot->SetAlignment(FVector2D(0,1));
        Slot->SetPosition(FVector2D(32,-30));
        Slot->SetAutoSize(true);
    }

    NexusText = WidgetTree->ConstructWidget<UTextBlock>(
        UTextBlock::StaticClass(), TEXT("Nexus"));
    Root->AddChild(NexusText);
    if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(NexusText->Slot))
    {
        Slot->SetAnchors(FAnchors(1,0,1,0));
        Slot->SetAlignment(FVector2D(1,0));
        Slot->SetPosition(FVector2D(-32,30));
        Slot->SetAutoSize(true);
    }
}

void UOmnisHUDWidget::RefreshHUD()
{
    APlayerController* PC = GetOwningPlayer();
    AOmnisPlayerCharacter* Player = PC
        ? Cast<AOmnisPlayerCharacter>(PC->GetPawn())
        : nullptr;

    FString Phase = TEXT("BOOT");
    int32 CycleIndex = 0;
    float WorldAge = 0.0f;

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UOmnisCycleSubsystem* Cycle =
            GI->GetSubsystem<UOmnisCycleSubsystem>())
        {
            const FOmnisCycleState State = Cycle->GetCycleState();
            CycleIndex = State.CycleIndex;
            WorldAge = State.WorldAge;

            switch (static_cast<uint8>(State.Phase))
            {
                case 0: Phase = TEXT("AWAKENING"); break;
                case 1: Phase = TEXT("EXPLORATION"); break;
                case 2: Phase = TEXT("CRISIS"); break;
                case 3: Phase = TEXT("COLLAPSE"); break;
                case 4: Phase = TEXT("NEXUS"); break;
                case 5: Phase = TEXT("REBIRTH"); break;
                default: break;
            }
        }
    }

    const float Health = Player && Player->Health
        ? Player->Health->GetHealth()
        : 0.0f;
    const float MaxHealth = Player && Player->Health
        ? Player->Health->GetMaxHealth()
        : 100.0f;

    if (WorldText)
    {
        WorldText->SetText(FText::FromString(FString::Printf(
            TEXT("CICLO %d | FASE %s | EDAD %.1f"),
            CycleIndex, *Phase, WorldAge)));
    }

    if (ProgressionText && GetGameInstance())
    {
        if (const UOmnisProgressionSubsystem* Progression =
            GetGameInstance()->GetSubsystem<UOmnisProgressionSubsystem>())
        {
            const FOmnisPlayerProgression Progress =
                Progression->GetProgression();
            ProgressionText->SetText(FText::FromString(FString::Printf(
                TEXT("NIVEL %d | XP %d"),
                Progress.Level, Progress.Experience)));
        }
    }

    if (SocialText && GetGameInstance())
    {
        const UOmnisPartySubsystem* Party =
            GetGameInstance()->GetSubsystem<UOmnisPartySubsystem>();
        const UOmnisRankedSubsystem* Ranked =
            GetGameInstance()->GetSubsystem<UOmnisRankedSubsystem>();

        const int32 PartySize = Party ? Party->GetMembers().Num() : 0;
        const FOmnisRankedState RankedState =
            Ranked ? Ranked->GetRankedState() : FOmnisRankedState();
        const FString Division =
            Ranked ? Ranked->GetDivisionName().ToString() : TEXT("Bronce");

        SocialText->SetText(FText::FromString(FString::Printf(
            TEXT("GRUPO %d/8 | RANGO %s | %d"),
            PartySize, *Division, RankedState.Rating)));
    }

    if (HealthText)
    {
        HealthText->SetText(FText::FromString(FString::Printf(
            TEXT("VITALIDAD %.0f / %.0f"), Health, MaxHealth)));
    }

    if (HealthBar)
    {
        HealthBar->SetPercent(
            MaxHealth > 0.0f
                ? FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f)
                : 0.0f);
    }

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UOmnisNexusSubsystem* Nexus =
            GI->GetSubsystem<UOmnisNexusSubsystem>())
        {
            if (NexusText)
            {
                NexusText->SetText(FText::FromString(FString::Printf(
                    TEXT("NEXUS %d PTOS | VITALIDAD +%d"),
                    Nexus->GetPermanentUpgradePoints(),
                    Nexus->GetPermanentVitalityLevel())));
            }
        }
    }
}
