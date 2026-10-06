#include "Gameplay/Combat/OmnisCombatComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UOmnisCombatComponent::UOmnisCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UOmnisCombatComponent::Attack()
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World)
    {
        return false;
    }

    const FVector Start = Owner->GetActorLocation() + FVector::UpVector * 50.0f;
    const FVector End = Start + Owner->GetActorForwardVector() * AttackRange;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(OmnisAttack), false, Owner);
    if (!World->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
    {
        return false;
    }

    AActor* Target = Hit.GetActor();
    if (!Target)
    {
        return false;
    }

    UGameplayStatics::ApplyDamage(Target, AttackDamage, Owner->GetInstigatorController(), Owner, UDamageType::StaticClass());
    OnAttackResolved.Broadcast(Target, AttackDamage);
    return true;
}
