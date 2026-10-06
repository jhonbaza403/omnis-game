#include "Online/Accounts/OmnisAccountSubsystem.h"

void UOmnisAccountSubsystem::SetDisplayName(FString NewName)
{
    NewName.TrimStartAndEndInline();
    DisplayName = NewName.Left(32);
}
