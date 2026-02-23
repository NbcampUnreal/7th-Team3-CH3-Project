// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Shared/ItemTypes.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UParkourComponent;
class UStatComponent;
class AWeaponItem;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle, Walking, Sprinting, Parkour, Dead
};

UENUM(BlueprintType)
enum class EOverlayState : uint8 
{
	Default, // 맨손
	Melee,   // 근접 무기
	Pistol,  // 권총
	Rifle    // 소총
};

UCLASS(Blueprintable, BlueprintType)
class TEAM3_PROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* SandboxMappingContext;

public:
	APlayerCharacter();

public:
	// --- 상태 변경 ---
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetCharacterState(ECharacterState NewState);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECharacterState CurrentState;

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	//FOnStateChanged OnStateChanged;

	// --- 컴포넌트 --- 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStatComponent* StatComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UParkourComponent* ParkourComp;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	//UInventoryComponent* InventoryComponent;

	// --- 무기 & 전투 관련 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType CurrentOverlayState = EWeaponType::WT_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AWeaponItem* CurrentWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName = FName("hand_r_weapon");

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartAiming();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopAiming();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack(); // -> CurrentWeapon->Fire() 호출

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Reload(); // -> CurrentWeapon->Reload() 호출

	UFUNCTION(BlueprintCallable, Category = "Weapon")
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

private:
	FName CurrentWeaponItemID = NAME_None;
	TSharedPtr<struct FStreamableHandle> WeaponLoadHandle;
	void OnWeaponClassLoaded(FName ItemID); // 무기 로드 완료됐을때 콜백함수

};
