// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team3_ProjectCharacter.h"
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

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATeam3_ProjectCharacter

ATeam3_ProjectCharacter::ATeam3_ProjectCharacter()
{
	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	
	CurrentState = ECharacterState::Idle;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATeam3_ProjectCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATeam3_ProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 조준
		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ATeam3_ProjectCharacter::StartAiming);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATeam3_ProjectCharacter::StopAiming);
		}

		// 공격
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ATeam3_ProjectCharacter::Attack);
		}

		// 상호작용
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATeam3_ProjectCharacter::TryInteract);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void ATeam3_ProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StatComp)
	{
		StatComp->InitializeStat("Stamina", MaxStamina, 0.f, MaxStamina);
	}
}

void ATeam3_ProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintDebugInfo();
}

void ATeam3_ProjectCharacter::SetCharacterState(ECharacterState NewState)
{
	// 상태가 같으면 변경하지 않음
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
}

void ATeam3_ProjectCharacter::StartAiming()
{
}

void ATeam3_ProjectCharacter::StopAiming()
{
}

void ATeam3_ProjectCharacter::Attack()
{
}

void ATeam3_ProjectCharacter::Reload()
{
}

void ATeam3_ProjectCharacter::EquipWeapon(FName ItemID)
{
}

void ATeam3_ProjectCharacter::StartSprint()
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
		&ATeam3_ProjectCharacter::HandleSprintCost,
		0.1f,
		true // Loop 여부
	);
}

void ATeam3_ProjectCharacter::StopSprint()
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
			&ATeam3_ProjectCharacter::HandleStaminaRecovery,
			0.1f,
			true
		);
	}
}

void ATeam3_ProjectCharacter::HandleSprintCost()
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

void ATeam3_ProjectCharacter::HandleStaminaRecovery()
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

void ATeam3_ProjectCharacter::TryInteract()
{
}

float ATeam3_ProjectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}

void ATeam3_ProjectCharacter::Die()
{
}

void ATeam3_ProjectCharacter::PrintDebugInfo()
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