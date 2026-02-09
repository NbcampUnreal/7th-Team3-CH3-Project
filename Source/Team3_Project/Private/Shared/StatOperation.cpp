#include "Shared/StatOperation.h"
#include "Shared/Component/StatComponent.h"

UStatOperation::UStatOperation()
{
}

void UStatOperation::Initialize(FName StatName, EStatOperator InOperator, float InValue, UObject* InSource)
{
    TargetStatName = StatName;
    Operator = InOperator;
    Value = InValue;
    Source = InSource;
}

void UStatOperation::ApplyOperation(UStatComponent* TargetComponent)
{
    if (TargetComponent != nullptr)
    {
        TargetComponent->AddOperation(TargetStatName, this);
    }
}

void UStatOperation::RemoveOperation(UStatComponent* TargetComponent)
{
    if (TargetComponent != nullptr)
    {
        TargetComponent->RemoveOperation(TargetStatName, this);
    }
}

FName UStatOperation::GetTargetStatName() const
{
    return TargetStatName;
}

EStatOperator UStatOperation::GetOperator() const
{
    return Operator;
}

float UStatOperation::GetValue() const
{
    return Value;
}

UObject* UStatOperation::GetSource() const
{
    return Source;
}

void UStatOperation::SetValue(float NewValue)
{
    Value = NewValue;
}