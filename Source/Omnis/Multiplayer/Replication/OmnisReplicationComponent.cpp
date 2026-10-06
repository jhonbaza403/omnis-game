#include "Multiplayer/Replication/OmnisReplicationComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

UOmnisReplicationComponent::UOmnisReplicationComponent()
{
    SetIsReplicatedByDefault(true);
}

void UOmnisReplicationComponent::SetAuthoritativeValue(int32 Value)
{
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        AuthoritativeValue = Value;
    }
    else
    {
        ServerSetAuthoritativeValue(Value);
    }
}

void UOmnisReplicationComponent::ServerSetAuthoritativeValue_Implementation(int32 Value)
{
    AuthoritativeValue = Value;
}

void UOmnisReplicationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UOmnisReplicationComponent, AuthoritativeValue);
}
