#include "Stat.h"

FStat::FStat()
    :BaseValue(0.0f),
    CurrentValue(0.0f),
    MinValue(0.0f),
    MaxValue(0.0f)
{
}

FStat::FStat(float Base, float Min, float Max)
    :BaseValue(Base),
    CurrentValue(Base),
    MinValue(Min),
    MaxValue(Max)
{
}

// Getter
float FStat::GetBaseValue() const
{
    return BaseValue;
}

float FStat::GetCurrentValue() const
{
    return CurrentValue;
}

float FStat::GetMinValue() const
{
    return MinValue;
}

float FStat::GetMaxValue() const
{
    return MaxValue;
}

float FStat::GetPercentage() const
{
    return CurrentValue / MaxValue;
}

// Setter
void FStat::SetBaseValue(float Value)
{
    BaseValue = FMath::Clamp(BaseValue, MinValue, MaxValue);
}

void FStat::SetCurrentValue(float Value)
{
    CurrentValue = FMath::Clamp(CurrentValue, MinValue, MaxValue);
}

void FStat::ModifyCurrentValue(float Delta)
{
    // Todo 이 함수가 어쩌다 만들어졌는 지 다시 파악할 필요가 있음.
}

void FStat::ResetToMax()
{
    CurrentValue = MaxValue;
}

void FStat::ResetToBase()
{
    CurrentValue = BaseValue;
}