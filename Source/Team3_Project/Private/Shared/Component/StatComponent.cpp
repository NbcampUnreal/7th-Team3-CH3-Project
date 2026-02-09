#include "Shared/Component/StatComponent.h"
#include "Shared/Stat.h"
#include "Shared/StatOperation.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


bool UStatComponent::InitializeStat(FName StatName, float Base, float Min, float Max)
{
	if (Stats.Contains(StatName)) return false;

	FStat NewStat(Base, Min, Max);
	Stats.Add(StatName, NewStat);
	SubOperations.Add(StatName, TArray<UStatOperation*>());

	return true;
}


bool UStatComponent::HasStat(FName StatName) const
{
	return Stats.Contains(StatName);
}

float UStatComponent::GetCurrentStatValue(FName StatName) const
{
	if (!Stats.Contains(StatName)) return 0.f;

	return Stats[StatName].GetCurrentValue();
}

float UStatComponent::GetBaseStatValue(FName StatName) const
{
	if (!Stats.Contains(StatName)) return 0.f;

	return Stats[StatName].GetBaseValue();
}

float UStatComponent::GetMinStatValue(FName StatName) const
{
	if (!Stats.Contains(StatName)) return 0.f;

	return Stats[StatName].GetMinValue();
}

float UStatComponent::GetMaxStatValue(FName StatName) const
{
	if (!Stats.Contains(StatName)) return 0.f;

	return Stats[StatName].GetMaxValue();
}

float UStatComponent::GeteStatPercentage(FName StatName) const
{
	if (!Stats.Contains(StatName)) return 0.f;

	return Stats[StatName].GetPercentage();
}

FStat UStatComponent::GetStat(FName StatName)
{
	if (!Stats.Contains(StatName)) return FStat();

	return Stats[StatName];
}

bool UStatComponent::SetCurrentStatValue(FName StatName, float Value)
{
	if (!Stats.Contains(StatName)) return false;

	Stats[StatName].SetCurrentValue(Value);
	return true;
}

bool UStatComponent::SetBaseStatValue(FName StatName, float Value)
{
	if (!Stats.Contains(StatName)) return false;

	Stats[StatName].SetBaseValue(Value);
	return true;
}

bool UStatComponent::ResetStatToMax(FName StatName)
{
	if (!Stats.Contains(StatName)) return false;

	Stats[StatName].ResetToMax();
	return true;
}

bool UStatComponent::ResetStatToBase(FName StatName)
{
	if (!Stats.Contains(StatName)) return false;

	Stats[StatName].ResetToBase();
	return true;
}

bool UStatComponent::AddOperation(FName StatName, UStatOperation* Oper)
{
	if (!SubOperations.Contains(StatName)) return false;

	SubOperations[StatName].Add(Oper);
	UpdateStat(StatName);
	return true;
}

bool UStatComponent::RemoveOperation(FName StatName, UStatOperation* Oper)
{
	if (!SubOperations.Contains(StatName)) return false;

	SubOperations[StatName].Remove(Oper);
	UpdateStat(StatName);
	return true;
}


void UStatComponent::RemoveOperationBySource(UObject* Source)
{
	// Todo 로직 개선
	TArray<TPair<FName, UStatOperation*>> RemovedOper;

	for (auto& OperationsRow : SubOperations)
	{
		TArray<UStatOperation*>& Operations = OperationsRow.Value;
		for (auto& Oper : Operations)
		{
			if (Oper->GetSource() == Source)
			{
				RemovedOper.Add({ OperationsRow.Key, Oper });
			}
		}
	}

	for (auto& Pair : RemovedOper)
	{
		RemoveOperation(Pair.Key, Pair.Value);
	}
}

void UStatComponent::UpdateStat(FName StatName)
{
	// Reset Stat
	Stats[StatName].ResetToBase();

	// Todo 스탯 계산 공식에 따라 스텟 계산하기
}