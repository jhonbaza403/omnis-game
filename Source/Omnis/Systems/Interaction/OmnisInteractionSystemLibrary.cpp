#include "Systems/Interaction/OmnisInteractionSystemLibrary.h"
#include "Gameplay/Interaction/OmnisInteractableInterface.h"

bool UOmnisInteractionSystemLibrary::IsInteractable(AActor* Actor)
{
    return Actor && Actor->GetClass()->ImplementsInterface(UOmnisInteractableInterface::StaticClass());
}
