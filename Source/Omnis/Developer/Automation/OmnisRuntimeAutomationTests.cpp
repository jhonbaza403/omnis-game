#include "Misc/AutomationTest.h"

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Gameplay/Player/OmnisGameMode.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Gameplay/Player/OmnisPlayerController.h"
#include "UI/HUD/OmnisHUD.h"
#include "World/Environment/OmnisNexusVisualActor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FOmnisRuntimeFoundationTest,
    "Omnis.Runtime.Foundation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FOmnisRuntimeFoundationTest::RunTest(const FString& Parameters)
{
    TestNotNull(TEXT("Omnis GameMode"), AOmnisGameMode::StaticClass());

    const AOmnisGameMode* Defaults = GetDefault<AOmnisGameMode>();
    TestEqual(TEXT("Default pawn class"), Defaults->DefaultPawnClass, AOmnisPlayerCharacter::StaticClass());
    TestEqual(TEXT("Player controller class"), Defaults->PlayerControllerClass, AOmnisPlayerController::StaticClass());
    TestEqual(TEXT("HUD class"), Defaults->HUDClass, AOmnisHUD::StaticClass());

    TestNotNull(TEXT("Nexus visual actor"), AOmnisNexusVisualActor::StaticClass());
    TestNotNull(TEXT("Engine cube mesh"), LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));
    TestNotNull(TEXT("Engine sphere mesh"), LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")));

    const FString GeneratorPath = FPaths::ProjectDir() / TEXT("Tools/Editor/GenerateOmnisVerticalSlice.py");
    TestTrue(TEXT("Vertical slice generator exists"), FPaths::FileExists(GeneratorPath));

    const FString ManifestPath = FPaths::ProjectDir() / TEXT("Content/Omnis/World/world-manifest.json");
    FString Manifest;
    TestTrue(TEXT("World manifest exists"), FFileHelper::LoadFileToString(Manifest, *ManifestPath));
    TestTrue(TEXT("World manifest declares PrimerCiclo"), Manifest.Contains(TEXT("PrimerCiclo")));

    return true;
}
