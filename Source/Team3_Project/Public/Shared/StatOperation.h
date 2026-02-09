#pragma once

#include "CoreMinimal.h"
#include "StatOperation.generated.h"

class UStatComponent;

UENUM(BlueprintType)
enum class EStatOperator : uint8
{
	Plus,
	Multiply
};

UCLASS()
class TEAM3_PROJECT_API UStatOperation : public UObject
{
	GENERATED_BODY()

public:
	UStatOperation();

	void Initialize(FName StatName, EStatOperator InOperator, float InValue, UObject* InSource);
	void ApplyOperation(UStatComponent* TargetComponent);
	void RemoveOperation(UStatComponent* TargetComponent);
	FName GetTargetStatName() const;
	EStatOperator GetOperator() const;
	float GetValue() const;
	UObject* GetSource() const;
	void SetValue(float NewValue);

protected:
	FName TargetStatName;
	EStatOperator Operator;
	float Value;
	TObjectPtr<UObject> Source;
};
