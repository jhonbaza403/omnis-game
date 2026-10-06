#include "Modules/ModuleManager.h"

class FOmnisWorldModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FOmnisWorldModule, OmnisWorld);
