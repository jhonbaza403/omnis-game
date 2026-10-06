#include "Online/Content/OmnisTitleUpdateSubsystem.h"

#include "Interfaces/OnlineTitleFileInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"

bool UOmnisTitleUpdateSubsystem::EnumerateUpdateFiles()
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineTitleFilePtr TitleFile = OnlineSubsystem->GetTitleFileInterface();
    if (!TitleFile.IsValid())
    {
        return false;
    }

    return TitleFile->EnumerateFiles(FPagedQuery(0, -1));
}

bool UOmnisTitleUpdateSubsystem::ReadUpdateFile(const FString& FileName)
{
    if (FileName.TrimStartAndCopy().IsEmpty())
    {
        return false;
    }

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineTitleFilePtr TitleFile = OnlineSubsystem->GetTitleFileInterface();
    return TitleFile.IsValid() && TitleFile->ReadFile(FileName);
}

bool UOmnisTitleUpdateSubsystem::GetUpdateFileContents(const FString& FileName, TArray<uint8>& OutContents) const
{
    OutContents.Reset();

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineTitleFilePtr TitleFile = OnlineSubsystem->GetTitleFileInterface();
    return TitleFile.IsValid() && TitleFile->GetFileContents(FileName, OutContents);
}
