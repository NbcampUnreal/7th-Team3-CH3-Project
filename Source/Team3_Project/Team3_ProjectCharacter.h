// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Team3_ProjectCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UParkourComponent;
class UStatComponent;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle, Walking, Sprinting, Parkour, Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, ECharacterState, NewState);
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ATeam3_ProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

public:
	ATeam3_ProjectCharacter();

public:
	// --- 상태 변경 ---
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetCharacterState(ECharacterState NewState);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECharacterState CurrentState;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateChanged OnStateChanged;

	// --- 컴포넌트 --- 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStatComponent* StatComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UParkourComponent* ParkourComp;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	//UInventoryComponent* InventoryComponent;

	// --- 무기 & 전투 관련 ---
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	//AWeaponItem* CurrentWeapon;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartAiming();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopAiming();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack(); // -> CurrentWeapon->Fire() 호출

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Reload(); // -> CurrentWeapon->Reload() 호출

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipWeapon(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopSprint();

	// --- 상호작용 ---
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	// --- 데미지 & 죽음 ---
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

	// --- Input Actions ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InteractAction;

	// --- 스탯 값들 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Config")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Config")
	float SprintCostPerSecond = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Config")
	float StaminaRecoveryRate = 20.f; 

	void PrintDebugInfo(); // 임시 디버그용 -> 곧 삭제

protected:
	// 조준 중인지 확인 (상태와 별개로 체크 - 추후 상하체 동작 분리 위해..아마..?)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsAiming;

	FTimerHandle SprintTimerHandle;
	FTimerHandle StaminaRecoveryTimerHandle; // 회복용
	void HandleSprintCost();
	void HandleStaminaRecovery();
	
protected:
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};