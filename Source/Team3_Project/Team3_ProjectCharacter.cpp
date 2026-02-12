// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team3_ProjectCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATeam3_ProjectCharacter

ATeam3_ProjectCharacter::ATeam3_ProjectCharacter()
{
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
