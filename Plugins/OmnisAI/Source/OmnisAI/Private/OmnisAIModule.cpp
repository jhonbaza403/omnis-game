#include "Modules/ModuleManager.h"

class FOmnisAIModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FOmnisAIModule, OmnisAI);
