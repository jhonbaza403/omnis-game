#include "Gameplay/Combat/OmnisCombatComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UOmnisCombatComponent::UOmnisCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

bool UOmnisCombatComponent::Attack()
{
    AActor* Owner = GetOwner();
    if (!Owner || !GetWorld())
    {
        return false;
    }

    if (Owner->HasAuthority())
    {
        return PerformAttackOnAuthority();
    }

    ServerAttack();
    return true;
}

void UOmnisCombatComponent::ServerAttack_Implementation()
{
    PerformAttackOnAuthority();
}

bool UOmnisCombatComponent::PerformAttackOnAuthority()
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World || !Owner->HasAuthority())
    {
        return false;
    }

    const float SafeRange = FMath::Clamp(
        FMath::IsFinite(AttackRange) ? AttackRange : 280.0f,
        50.0f,
        500.0f);
    const float SafeDamage = FMath::Clamp(
        FMath::IsFinite(AttackDamage) ? AttackDamage : 25.0f,
        0.0f,
        500.0f);
    if (SafeDamage <= 0.0f)
    {
        return false;
    }

    const FVector Start = Owner->GetActorLocation() + FVector::UpVector * 50.0f;
    const FVector End = Start + Owner->GetActorForwardVector() * SafeRange;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(OmnisAttack), false, Owner);
    if (!World->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
    {
        return false;
    }

    AActor* Target = Hit.GetActor();
    if (!Target || FVector::DistSquared(Owner->GetActorLocation(), Target->GetActorLocation()) >
        FMath::Square(SafeRange))
    {
        return false;
    }

    UGameplayStatics::ApplyDamage(
        Target,
        SafeDamage,
        Owner->GetInstigatorController(),
        Owner,
        UDamageType::StaticClass());
    OnAttackResolved.Broadcast(Target, SafeDamage);
    return true;
}
