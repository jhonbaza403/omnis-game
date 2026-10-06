#include "Developer/Automation/OmnisAutomationLibrary.h"

bool UOmnisAutomationLibrary::IsValidIdentifier(FName Identifier)
{
    return !Identifier.IsNone() && !Identifier.ToString().Contains(TEXT(" "));
}
