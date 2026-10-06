#include "Audio/SFX/OmnisSFXLibrary.h"

float UOmnisSFXLibrary::ComputeAudibility(float BaseVolume, float Distance, float MaxDistance)
{
    if (MaxDistance <= 0.0f)
    {
        return 0.0f;
    }

    const float Falloff = 1.0f - FMath::Clamp(Distance / MaxDistance, 0.0f, 1.0f);
    return FMath::Clamp(BaseVolume, 0.0f, 1.0f) * Falloff;
}
