#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpecialAttackComponent.generated.h"

class USpecialAttackData;
class USpecialAttackBase;
class AEnemyCharacter;

/**
 * 특수 공격 런타임 상태 관리 컴포넌트
 * - DataAsset은 읽기 전용 유지
 * - 쿨타임 등 런타임 상태만 컴포넌트에서 관리
 * - Boss Phase 시스템 지원
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAM3_PROJECT_API USpecialAttackComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USpecialAttackComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ========================================
    // Phase 관리
    // ========================================
    
    /** 현재 Phase의 DataAsset 설정 */
    UFUNCTION(BlueprintCallable, Category = "Special Attack")
    void SetSpecialAttackData(USpecialAttackData* NewData);

    /** Phase 전환 (Boss용) */
    UFUNCTION(BlueprintCallable, Category = "Special Attack")
    void SwitchPhase(int32 PhaseIndex);

    // ========================================
    // 공격 실행
    // ========================================
    
    /** ID로 특수 공격 실행 */
    UFUNCTION(BlueprintCallable, Category = "Special Attack")
    bool ExecuteSpecialAttackByID(FName AttackID, AActor* TargetActor);

    /** 현재 실행 중인 특수 공격 효과 발동 */
    UFUNCTION(BlueprintCallable, Category = "Special Attack")
    void TriggerSpecialAttackEffect();

    /** 특수 공격 완료 */
    UFUNCTION(BlueprintCallable, Category = "Special Attack")
    void OnFinishSpecialAttack();

    // ========================================
    // 조회
    // ========================================
    
    /** DataAsset에서 공격 찾기 */
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    USpecialAttackBase* GetAttackByID(FName AttackID) const;

    /** 특정 공격이 준비되었는지 */
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    bool IsAttackReady(FName AttackID) const;

    /** 특정 공격의 남은 쿨타임 */
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    float GetCooldownLeft(FName AttackID) const;

    /** 사용 가능한 공격 목록 */
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    TArray<USpecialAttackBase*> GetAvailableAttacks(AActor* Target) const;

    /** 현재 진행 중인 공격 반환 */
    UFUNCTION(BlueprintPure, Category = "Special Attack")
    USpecialAttackBase* GetCurrentAttack() const;

protected:
    /** Owner Enemy 캐싱 */
    UPROPERTY()
    AEnemyCharacter* OwnerEnemy;

    /** 현재 사용 중인 SpecialAttackData (읽기 전용) */
    UPROPERTY()
    USpecialAttackData* CurrentSpecialAttackData;

    /** Phase별 DataAsset 목록 (Boss용) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Special Attack|Phases")
    TArray<USpecialAttackData*> PhaseDataAssets;

    /** 현재 Phase 인덱스 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Special Attack|Phases")
    int32 CurrentPhaseIndex = 0;

    // ========================================
    // 런타임 상태 (각 공격별 쿨타임)
    // ========================================
    
    /** 특수 공격별 남은 쿨타임 (AttackID → LeftCoolTime) */
    UPROPERTY()
    TMap<FName, float> AttackCooldowns;

    /** 현재 실행 중인 특수 공격 */
    UPROPERTY()
    USpecialAttackBase* CurrentSpecialAttack = nullptr;

    /** 현재 공격 대상 */
    UPROPERTY()
    AActor* CurrentTargetActor = nullptr;
};