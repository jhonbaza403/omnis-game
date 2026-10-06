#include "Modules/ModuleManager.h"

class FOmnisOnlineModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FOmnisOnlineModule, OmnisOnline);
