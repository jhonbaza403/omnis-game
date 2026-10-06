#include "Modules/ModuleManager.h"

class FOmnisCoreModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FOmnisCoreModule, OmnisCore);
