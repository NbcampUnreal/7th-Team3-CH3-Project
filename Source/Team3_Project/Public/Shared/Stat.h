#pragma once

#include "CoreMinimal.h"
#include "Stat.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FStat
{
    GENERATED_BODY()

public:
    // Constructor
    FStat();
    FStat(float Base, float Min, float Max);

    // Getter
    float GetBaseValue() const;
    float GetCurrentValue() const;
    float GetMinValue() const;
    float GetMaxValue() const;
    float GetPercentage() const;

    // Setter
    void SetBaseValue(float Value);
    void SetCurrentValue(float Value);

    void ResetToMax();
    void ResetToBase();

protected:
    float BaseValue;
    float CurrentValue;
    float MinValue;
    float MaxValue;
};