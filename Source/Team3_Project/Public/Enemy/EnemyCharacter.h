#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/EnemyTypeData.h"
#include "Enemy/SpecialAttackData.h"
#include "EnemyCharacter.generated.h"

class UStatComponent;
class USphereComponent;
class UDamageType;
struct FDamageEvent;
class USpecialAttackData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovableChanged, bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveMode, bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishSpecialAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHitted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishHitted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishDead);

UCLASS()
class TEAM3_PROJECT_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	bool DropItem();
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool Attack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool SpecialAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFinishAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFinishSpecialAttack();

	// 근접 공격 콜리전 제어
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DisableWeaponCollision();
	
	// 공격 시 호출
	void ResetHitActors();
	
	// ID로 특수 공격 실행
	UFUNCTION(BlueprintCallable, Category = "Combat|Special")
	bool ExecuteSpecialAttackByID(FName AttackID, AActor* TargetActor);

	// 특수 공격 효과 발동
	UFUNCTION(BlueprintCallable, Category = "Combat|Special")
	void TriggerSpecialAttackEffect();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnHitted();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFinishHitted();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnDead(bool bShouldPlayMontage);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFinishDead();

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInsitagtor,
		AActor* DamageCauser
	) override;

	// Getter
	UFUNCTION(BlueprintPure, Category = "Enemy")
	FName GetName() const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetAttack() const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetDefence() const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetWhiteKarma() const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetBlackKarma() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttackable();
	UFUNCTION(BlueprintPure, Category = "Move")
	bool IsMovable() const;
	UFUNCTION(BlueprintPure, Category = "Wave")
	bool IsForWave() const;
	UFUNCTION(BlueprintPure, Category="Dead")
	bool IsDead() const;
	UFUNCTION(BlueprintPure, Category = "Dead")
	bool IsRagdollEnabled() const;
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void ApplyWaveFlag(bool bInWave);

	void ActiveMove();
	void DeactiveMove();

	float GetAttackCoolTime() const;
	UAnimMontage* GetAttackMontage() const;
	UAnimMontage* GetSpecialAttackMontage() const;
	UAnimMontage* GetHittedMontage() const;
	UAnimMontage* GetDeadMontage() const;

	float GetPatrolSpeed() const;
	float GetPatrolRadius() const;
	float GetChaseSpeed() const;
	float GetAttackMoveSpeedRatio() const;
	float GetHittedMoveSpeedRatio() const;

	float GetItemDropRatio() const;
	TArray<FEnemyDropItem> GetDropItemTable() const;

	void ApplyDamageToStat(float DamageAmount);
	void EnableRagdoll();

	void SetForAIMove();

private:
	// 근접 공격 히트 처리
	UFUNCTION()
	void OnWeaponBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	// Delegates
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChangedSignature;
	UPROPERTY(BlueprintAssignable)
	FOnMovableChanged OnMovableChangedSignature;
	UPROPERTY(BlueprintAssignable)
	FOnWaveMode OnWaveModeSignature;
	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttackSignature;
	UPROPERTY(BlueprintAssignable)
	FOnFinishAttack OnFinishAttackSignature;
	UPROPERTY(BlueprintAssignable)
	FOnSpecialAttack OnSepcialAttackSignature;
	UPROPERTY(BlueprintAssignable)
	FOnFinishSpecialAttack OnFinishSpecialAttackSignature;
	UPROPERTY(BlueprintAssignable)
	FOnHitted OnHittedSignature;
	UPROPERTY(BlueprintAssignable)
	FOnFinishHitted OnFinishHittedSignature;
	UPROPERTY(BlueprintAssignable)
	FOnDead OnDeadSignature;
	UPROPERTY(BlueprintAssignable)
	FOnFinishDead OnFinishDeadSignature;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	UStatComponent* StatComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	FName EnemyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UEnemyTypeData> TypeData;

	// Special Attack Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<USpecialAttackData> SpecialAttackData;

	/** 현재 실행 중인 SpecialAttack */
	UPROPERTY()
	USpecialAttackBase* CurrentSpecialAttack = nullptr;

	/** 현재 공격 대상 */
	UPROPERTY()
	AActor* CurrentTargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	USphereComponent* WeaponCollision;
	
	// 중복 타격 방지
	TSet<AActor*> HitActorsThisAttack;

	float LeftAttackCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	bool bIsMovable;

	bool bIsForWave;
	bool bIsDead;
	bool bRagdollEnabled;
};
