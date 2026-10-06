#include "Systems/Attributes/OmnisAttributeComponent.h"

UOmnisAttributeComponent::UOmnisAttributeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UOmnisAttributeComponent::SetAttribute(FName AttributeId, float Value)
{
    if (!AttributeId.IsNone() && FMath::IsFinite(Value))
    {
        Attributes.Add(AttributeId, Value);
    }
}

void UOmnisAttributeComponent::AddAttribute(FName AttributeId, float Delta)
{
    if (!AttributeId.IsNone() && FMath::IsFinite(Delta))
    {
        Attributes.FindOrAdd(AttributeId) += Delta;
    }
}

float UOmnisAttributeComponent::GetAttribute(FName AttributeId, float DefaultValue) const
{
    const float* Value = Attributes.Find(AttributeId);
    return Value ? *Value : DefaultValue;
}
