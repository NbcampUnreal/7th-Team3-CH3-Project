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

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	ATeam3_ProjectCharacter();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

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
	UParkourComponent* ParkourComponent;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	//UStatComponent* StatComponent;
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

protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AimWalkSpeed = 200.f; // 조준하며 걸을 때

	// 조준 중인지 확인 (상태와 별개로 체크 - 추후 상하체 동작 분리 위해..아마..?)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsAiming;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();

	void OnStateEnter(ECharacterState NewState);
	void OnStateExit(ECharacterState OldState);

			
protected:
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	// 조준시 카메라 길이 및 시야각
	float DefaultBaseLookUpRate = 45.f;
	float AimArmLength = 150.f; // 조준 시 카메라 지지대 길이
	float DefaultArmLength = 300.f; // 평소 길이
};

