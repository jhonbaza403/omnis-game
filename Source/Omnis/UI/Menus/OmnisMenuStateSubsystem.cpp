#include "UI/Menus/OmnisMenuStateSubsystem.h"

void UOmnisMenuStateSubsystem::OpenMenu(FName MenuId)
{
    if (!MenuId.IsNone())
    {
        OpenMenuId = MenuId;
    }
}

void UOmnisMenuStateSubsystem::CloseMenu()
{
    OpenMenuId = NAME_None;
}
