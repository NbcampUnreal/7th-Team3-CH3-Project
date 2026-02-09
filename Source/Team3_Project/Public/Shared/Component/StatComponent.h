#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shared/Stat.h"
#include "StatComponent.generated.h"

class UStatOperation;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM3_PROJECT_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatComponent();

	// ========================================
	// 스탯 초기화
	// ========================================
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool InitializeStat(FName StatName, float Base, float Min, float Max);

	// ========================================
   // 조회
   // ========================================
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool HasStat(FName StatName) const;
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetCurrentStatValue(FName StatName) const;
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetBaseStatValue(FName StatName) const;
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMinStatValue(FName StatName) const;
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMaxStatValue(FName StatName) const;
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GeteStatPercentage(FName StatName) const;

	FStat GetStat(FName StatName);

	// ========================================
	// 직접 수정 (즉시 효과)
	// ========================================
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool SetCurrentStatValue(FName StatName, float Value);
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool SetBaseStatValue(FName StatName, float Value);
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool ResetStatToMax(FName StatName);
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool ResetStatToBase(FName StatName);

	// ========================================
	// Operation 관리 (지속 효과) - 내부용
	// ========================================
	bool AddOperation(FName StatName, UStatOperation* Oper);
	bool RemoveOperation(FName StatName, UStatOperation* Oper);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RemoveOperationBySource(UObject* Source);

protected:
	void UpdateStat(FName StatName);

protected:
	TMap<FName, FStat> Stats;
	TMap<FName, TArray<UStatOperation*>> SubOperations;
};
