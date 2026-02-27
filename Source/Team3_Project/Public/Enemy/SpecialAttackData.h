#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enemy/SpecialAttack/SpecialAttackBase.h"
#include "SpecialAttackData.generated.h"


UCLASS()
class TEAM3_PROJECT_API USpecialAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
    // 특수 공격 목록
    UPROPERTY(EditDefaultsOnly, Instanced, Category = "Special Attacks")
    TArray<USpecialAttackBase*> SpecialAttacks;

    // ID로 공격 찾기
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    USpecialAttackBase* GetAttackByID(FName AttackID) const
    {
        for (USpecialAttackBase* Attack : SpecialAttacks)
        {
            if (Attack && Attack->GetAttackID() == AttackID)
            {
                return Attack;
            }
        }
        return nullptr;
    }

    // 거리에 맞는 공격 찾기
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    USpecialAttackBase* GetAttackForDistance(float Distance) const
    {
        for (USpecialAttackBase* Attack : SpecialAttacks)
        {
            if (Attack && Distance >= Attack->GetMinRange() && Distance <= Attack->GetMaxRange())
            {
                return Attack;
            }
        }
        return nullptr;
    }

    // 사용 가능한 공격 모두 가져오기
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    TArray<USpecialAttackBase*> GetAvailableAttacks(class AEnemyCharacter* Owner, AActor* Target) const
    {
        TArray<USpecialAttackBase*> Available;

        for (USpecialAttackBase* Attack : SpecialAttacks)
        {
            if (Attack && Attack->CanExecute(Owner, Target))
            {
                Available.Add(Attack);
            }
        }

        return Available;
    }

    // 랜덤 공격 선택
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    USpecialAttackBase* GetRandomAvailableAttack(class AEnemyCharacter* Owner, AActor* Target) const
    {
        TArray<USpecialAttackBase*> Available = GetAvailableAttacks(Owner, Target);

        if (Available.Num() == 0) return nullptr;

        int32 Index = FMath::RandRange(0, Available.Num() - 1);
        return Available[Index];
    }
};
