#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/EnemyTypeData.h"
#include "EnemyCharacter.generated.h"

class UStatComponent;

UCLASS()
class TEAM3_PROJECT_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool Attack();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool SpecialAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFinishAttack();

	// 몽타주가 없어서 임시로 사용하는 데미지 처리
	void TryMeleeHit();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnHitted();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFinishHitted();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnDead();
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

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttackable();
	UFUNCTION(BlueprintPure, Category = "Move")
	bool IsMovable() const;
	UFUNCTION(BlueprintPure, Category = "Wave")
	bool IsForWave() const;
	UFUNCTION(BlueprintPure, Category="Dead")
	bool IsDead() const;
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

	void ApplyDamageToStat(float DamageAmount);
	void EnableRagdoll();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	UStatComponent* StatComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	FName EnemyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UEnemyTypeData> TypeData;

	float LeftAttackCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	bool bIsMovable;

	bool bIsForWave;
	bool bIsDead;
	bool bRagdollEnabled;
};
