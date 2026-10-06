#include "Modules/ModuleManager.h"

class FOmnisNarrativeModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FOmnisNarrativeModule, OmnisNarrative);
