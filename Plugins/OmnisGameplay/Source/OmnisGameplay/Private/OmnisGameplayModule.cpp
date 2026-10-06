#include "Modules/ModuleManager.h"

class FOmnisGameplayModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FOmnisGameplayModule, OmnisGameplay);
