// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Shared/Component/StatComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	CurrentState = ECharacterState::Idle;
}

void APlayerCharacter::SetCharacterState(ECharacterState NewState)
{
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StatComp)
	{
		StatComp->InitializeStat("Stamina", MaxStamina, 0.f, MaxStamina);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PrintDebugInfo();
}

void APlayerCharacter::NotifyControllerChanged()
{
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerCharacter::StartAiming()
{
}

void APlayerCharacter::StopAiming()
{
}

void APlayerCharacter::Attack()
{
}

void APlayerCharacter::Reload()
{
}

void APlayerCharacter::EquipWeapon(FName ItemID)
{
}

void APlayerCharacter::StartSprint()
{
	// 달리기 시작하니까 회복 타이머 중지
	GetWorld()->GetTimerManager().ClearTimer(StaminaRecoveryTimerHandle);
	// 이미 타이머가 돌고 있으면 무시
	if (GetWorld()->GetTimerManager().IsTimerActive(SprintTimerHandle)) return;

	CurrentState = ECharacterState::Sprinting;

	// 타이머 시작 (0.1초마다 HandleSprintCost 반복 실행)
	GetWorld()->GetTimerManager().SetTimer(
		SprintTimerHandle,
		this,
		&APlayerCharacter::HandleSprintCost,
		0.1f,
		true // Loop 여부
	);
}

void APlayerCharacter::StopSprint()
{
	CurrentState = ECharacterState::Walking;

	// 소모 타이머 삭제
	GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);

	float Current = StatComp->GetCurrentStatValue("Stamina");
	if (Current < MaxStamina)
	{
		GetWorld()->GetTimerManager().SetTimer(
			StaminaRecoveryTimerHandle,
			this,
			&APlayerCharacter::HandleStaminaRecovery,
			0.1f,
			true
		);
	}
}

void APlayerCharacter::TryInteract()
{
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}

void APlayerCharacter::PrintDebugInfo()
{
	if (!StatComp) return;

	float Current = StatComp->GetCurrentStatValue("Stamina");

	FString StateStr = (CurrentState == ECharacterState::Sprinting) ? TEXT("RUNNING >>") : TEXT("Walking..");
	FColor TextColor = (CurrentState == ECharacterState::Sprinting) ? FColor::Yellow : FColor::Green;

	// Key값이 0이면 메시지가 쌓이지 않고 한 줄에서 숫자만 바뀜 (갱신)
	FString Msg = FString::Printf(TEXT("[%s] Stamina: %.1f / %.0f"), *StateStr, Current, MaxStamina);

	if (GEngine)
	{
		// Key: 0 (고정된 위치), Time: 0.0f (즉시 갱신)
		GEngine->AddOnScreenDebugMessage(0, 0.0f, TextColor, Msg);
	}
}

void APlayerCharacter::HandleSprintCost()
{
	if (!StatComp) return;

	float CurrentStamina = StatComp->GetCurrentStatValue("Stamina");

	// 0.1초만큼의 비용 계산
	float Cost = SprintCostPerSecond * 0.1f;
	float NewStamina = CurrentStamina - Cost;

	if (NewStamina <= 0.f)
	{
		NewStamina = 0.f;
		StopSprint();
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, TEXT("탈진! (Stamina Depleted)"));
	}

	StatComp->SetCurrentStatValue("Stamina", NewStamina);
}

void APlayerCharacter::HandleStaminaRecovery()
{
	if (!StatComp) return;

	float Current = StatComp->GetCurrentStatValue("Stamina");

	if (Current >= MaxStamina)
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRecoveryTimerHandle);
		return;
	}

	// 회복 계산 (초당 회복량 * 0.1초)
	float Recovery = StaminaRecoveryRate * 0.1f;
	float NewValue = Current + Recovery;

	if (NewValue > MaxStamina) NewValue = MaxStamina;
	StatComp->SetCurrentStatValue("Stamina", NewValue);
}

