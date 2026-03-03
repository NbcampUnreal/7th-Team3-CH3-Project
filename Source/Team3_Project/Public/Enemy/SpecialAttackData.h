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
    // 전체 쿨타임 Tick
    UFUNCTION(BlueprintCallable, Category = "Special Attack")
    void TickCooldowns(float DeltaTime);

    // 특수 공격 목록
    UPROPERTY(EditDefaultsOnly, Instanced, Category = "Special Attacks")
    TArray<USpecialAttackBase*> SpecialAttacks;

    // ID로 공격 찾기
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    USpecialAttackBase* GetAttackByID(FName AttackID) const;

    // 거리에 맞는 공격 찾기
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    USpecialAttackBase* GetAttackForDistance(float Distance) const;

    // 사용 가능한 공격 모두 가져오기
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    TArray<USpecialAttackBase*> GetAvailableAttacks(class AEnemyCharacter* Owner, AActor* Target) const;

    // 랜덤 공격 선택
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    USpecialAttackBase* GetRandomAvailableAttack(class AEnemyCharacter* Owner, AActor* Target) const;

    // 특정 공격의 남은 쿨타임
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    float GetCooldownLeft(FName AttackID) const;

    // 특정 공격이 준비되었는지
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    bool IsAttackReady(FName AttackID) const;
};
