#include "Systems/Localization/OmnisLocalizationSubsystem.h"

#include "Internationalization/Internationalization.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
    const TCHAR* LocalizationSection = TEXT("Omnis.Localization");
}

bool UOmnisLocalizationSubsystem::SetCulture(const FString& Culture)
{
    const FString Normalized = Culture.TrimStartAndCopy();
    if (Normalized.IsEmpty() || !GetSupportedCultures().Contains(Normalized))
    {
        return false;
    }

    return FInternationalization::Get().SetCurrentCulture(Normalized);
}

FString UOmnisLocalizationSubsystem::GetCurrentCulture() const
{
    return FInternationalization::Get().GetCurrentCulture()->GetName();
}

TArray<FString> UOmnisLocalizationSubsystem::GetSupportedCultures() const
{
    return LoadSupportedCultures();
}

FString UOmnisLocalizationSubsystem::GetNativeCulture() const
{
    FString Culture = TEXT("es");
    GConfig->GetString(LocalizationSection, TEXT("NativeCulture"), Culture, GGameIni);
    return Culture;
}

TArray<FString> UOmnisLocalizationSubsystem::LoadSupportedCultures() const
{
    FString Serialized;
    GConfig->GetString(LocalizationSection, TEXT("SupportedCultures"), Serialized, GGameIni);

    TArray<FString> Cultures;
    Serialized.ParseIntoArray(Cultures, TEXT(","), true);

    for (FString& Culture : Cultures)
    {
        Culture.TrimStartAndEndInline();
    }

    Cultures.RemoveAll([](const FString& Culture)
    {
        return Culture.IsEmpty();
    });

    const FString NativeCulture = GetNativeCulture();
    if (!NativeCulture.IsEmpty() && !Cultures.Contains(NativeCulture))
    {
        Cultures.Add(NativeCulture);
    }

    return Cultures;
}
