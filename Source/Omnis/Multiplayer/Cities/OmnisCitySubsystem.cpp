#include "Multiplayer/Cities/OmnisCitySubsystem.h"

void UOmnisCitySubsystem::RegisterCity(const FOmnisCity& City)
{
    if (!City.CityId.IsNone())
    {
        Cities.Add(City.CityId, City);
    }
}

FOmnisCity UOmnisCitySubsystem::GetCity(FName CityId) const
{
    const FOmnisCity* City = Cities.Find(CityId);
    return City ? *City : FOmnisCity{};
}
