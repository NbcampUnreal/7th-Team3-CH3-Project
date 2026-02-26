#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpecialAttackBase.generated.h"

class AEnemyCharacter;

UCLASS(Abstract, Blueprintable, EditInlineNew)
class TEAM3_PROJECT_API USpecialAttackBase : public UObject
{
	GENERATED_BODY()
public:
    //특수 공격 실행
    // Owner - 공격하는 Enemy
    // TargetActor - 공격 대상 (nullptr 가능)

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Special Attack")
    void Execute(AEnemyCharacter* Owner, AActor* TargetActor);
    virtual void Execute_Implementation(AEnemyCharacter* Owner, AActor* TargetActor);

    //공격 가능 여부 체크
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Special Attack")
    bool CanExecute(AEnemyCharacter* Owner, AActor* TargetActor) const;
    virtual bool CanExecute_Implementation(AEnemyCharacter* Owner, AActor* TargetActor) const;

    // Getters
    // 고유 ID
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    FName GetAttackID() const { return AttackID; }

    // 최소 거리
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    float GetMinRange() const { return MinRange; }

    // 최대 거리
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    float GetMaxRange() const { return MaxRange; }

    // 쿨다운
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    float GetCooldown() const { return Cooldown; }

    // 몽타주
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    UAnimMontage* GetMontage() const { return Montage; }

    // 이동 가능 여부
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    bool CanMoveWhileAttacking() const { return bCanMoveWhileAttacking; }

protected:
    // 고유 ID (예: "Fireball", "GroundSlam")
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    FName AttackID = FName("DefaultAttack");

    // 최소 사용 거리
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Range")
    float MinRange = 0.f;

    // 최대 사용 거리
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Range")
    float MaxRange = 1000.f;

    // 쿨다운 (초)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    float Cooldown = 5.f;

    // 재생할 몽타주
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    UAnimMontage* Montage = nullptr;

    // 공격 중 이동 가능 여부
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
    bool bCanMoveWhileAttacking = false;
};
