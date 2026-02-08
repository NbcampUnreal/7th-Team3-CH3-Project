#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UStatComponent;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Moving,
	Chasing,
	Attacking,
	Hitting,
	Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEnemyStateChanged, EEnemyState, OldState, EEnemyState, NewState);

UCLASS()
class TEAM3_PROJECT_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void DetectPlayer();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void ChasePlayer();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SpecialAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFinishAttack();

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

	UFUNCTION(BlueprintPure, Category = "Enemy")
	EEnemyState GetState() const;

	ACharacter* GetDetectedTarget() const;

	// Setter
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ChangeState(EEnemyState NewState);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	UStatComponent* StatComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	FName EnemyName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	EEnemyState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float DetectionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float ChaseRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<ACharacter> DetectedTarget;
	
	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FEnemyStateChanged OnEnemyStateChanged;
	
	FTimerHandle DetectionTimerHandle;
};
