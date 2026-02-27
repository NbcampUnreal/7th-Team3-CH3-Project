// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Shared/Component/StatComponent.h"
#include "GameFramework/Controller.h"
#include "Item/WeaponItem.h"
#include "Core/ItemDataSubsystem.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Shared/InteractionInterface.h"
#include "Item/InventoryComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	CurrentState = ECharacterState::Idle;
	CurrentOverlayState = EWeaponType::WT_None;
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
		StatComp->InitializeStat("Health", MaxHealth, 0.f, MaxHealth);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PrintDebugInfo();
	UpdateInteractableFocus();
}

void APlayerCharacter::UpdateInteractableFocus()
{
	FVector TraceStart;
	FRotator TraceRot;

	if (GetController())
	{
		GetController()->GetPlayerViewPoint(TraceStart, TraceRot);
	}
	else
	{
		TraceStart = GetActorLocation();
		TraceRot = GetActorRotation();
	}

	FVector TraceEnd = TraceStart + (TraceRot.Vector() * InteractionRange);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	AActor* TargetActor = nullptr;

	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart, 
		TraceEnd, 
		ECC_Visibility, 
		QueryParams
	))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->Implements<UInteractionInterface>())
		{
			TargetActor = HitActor;
		}
	}

	if (!TargetActor)
	{
		FVector SweepStart = GetActorLocation();
		FCollisionShape SphereShape = FCollisionShape::MakeSphere(InteractionRadius);
		TArray<FHitResult> OverlapResults;

		if (GetWorld()->SweepMultiByChannel(
			OverlapResults,
			SweepStart,
			SweepStart,
			FQuat::Identity,
			ECC_Visibility,
			SphereShape,
			QueryParams
		))
		{
			float MinDistance = MAX_FLT;

			for (const FHitResult& Result : OverlapResults)
			{
				AActor* OverlappedActor = Result.GetActor();
				if (OverlappedActor && OverlappedActor->Implements<UInteractionInterface>())
				{
					float Distance = FVector::Dist(SweepStart, OverlappedActor->GetActorLocation());
					if (Distance < MinDistance)
					{
						MinDistance = Distance;
						TargetActor = OverlappedActor;
					}
				}
			}
		}
	}

	if (CurrentFocusItem != TargetActor)
	{
		if (CurrentFocusItem && CurrentFocusItem->Implements<UInteractionInterface>())
		{
			IInteractionInterface::Execute_SetInteractFocus(CurrentFocusItem, false);
		}

		CurrentFocusItem = TargetActor;

		if (CurrentFocusItem && CurrentFocusItem->Implements<UInteractionInterface>())
		{
			IInteractionInterface::Execute_SetInteractFocus(CurrentFocusItem, true);
		}
	}
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
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire(); // 연사/단사 로직은 WeaponItem 내부 타이머가 처리함
	}
}

void APlayerCharacter::StopAttack()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void APlayerCharacter::Reload()
{
}

void APlayerCharacter::EquipWeapon(FName ItemID)
{
	// 토글 : 같은 무기 다시 누르면 해제
	if (CurrentWeaponItemID == ItemID && CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
		CurrentWeaponItemID = NAME_None;
		CurrentOverlayState = EWeaponType::WT_None;
		return;
	}

	// 데이터 조회
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI) return;

	UItemDataSubsystem* Subsystem = GI->GetSubsystem<UItemDataSubsystem>();
	if (!Subsystem) return;

	FItemData Data = Subsystem->GetItemDataByID(ItemID);

	// 기존 무기 제거
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	// Weapon 타입이 아니거나 클래스가 없으면 스킵
	if (Data.ItemType != EItemType::IT_Weapon || Data.ItemClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon: Invalid item or no WeaponActorClass: %s"), *ItemID.ToString());
		return;
	}

	// 비동기 로드 (게임 안멈추고 백그라운드에서 로드)
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	WeaponLoadHandle = Streamable.RequestAsyncLoad(
		Data.ItemClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &APlayerCharacter::OnWeaponClassLoaded, ItemID)
	);
}

void APlayerCharacter::OnWeaponClassLoaded(FName ItemID)
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI) return;

	UItemDataSubsystem* Subsystem = GI->GetSubsystem<UItemDataSubsystem>();
	if (!Subsystem) return;

	FItemData Data = Subsystem->GetItemDataByID(ItemID);
	if (Data.ItemClass.IsNull()) return;

	TSubclassOf<AWeaponItem> WeaponClass = Data.ItemClass.Get();
	if (!WeaponClass) return;

	// SpawnActor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeaponItem* NewWeapon = GetWorld()->SpawnActor<AWeaponItem>(
		WeaponClass, FTransform::Identity, SpawnParams
	);
	if (!NewWeapon) return;

	// 소켓에 붙이기
	NewWeapon->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		WeaponSocketName
	);

	// WeaponSocketTransform이 있으면 오프셋 적용
	if (!Data.WeaponSocketTransform.Equals(FTransform::Identity))
	{
		NewWeapon->SetActorRelativeTransform(Data.WeaponSocketTransform);
	}

	// 무기 No Collision
	// NewWeapon->GetRootComponent()->SetCollisionProfileName(TEXT("NoCollision"));

	// 상태 업데이트 -> ABP가 이걸 읽어서 포즈 전환
	CurrentWeapon = NewWeapon;
	CurrentWeaponItemID = ItemID;
	CurrentOverlayState = Data.WeaponType; // EWeaponType 그대로 사용
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

void APlayerCharacter::ApplyAdrenaline(int32 Duration)
{
}

void APlayerCharacter::TryInteract()
{

	if (CurrentFocusItem && CurrentFocusItem->Implements<UInteractionInterface>())
	{
		IInteractionInterface::Execute_Interact(CurrentFocusItem, this);
	}
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

void APlayerCharacter::EquipItemByData(const FInventoryItem& ItemData, ESlotType SlotType)
{
	UGameInstance* GI = GetWorld()->GetGameInstance();
	if (!GI) return;
	UItemDataSubsystem* Subsystem = GI->GetSubsystem<UItemDataSubsystem>();
	if (!Subsystem) return;
	FName ItemID = ItemData.ItemID;
	FItemData Data = Subsystem->GetItemDataByID(ItemID);

	if (Data.ItemType == EItemType::IT_Weapon)
	{
		UClass* WeaponClass = Data.ItemClass.LoadSynchronous();
		if (!WeaponClass) return;
		FTransform SpawnTransform = GetActorTransform();
		AWeaponItem* SpawnedWeapon = GetWorld()->SpawnActorDeferred<AWeaponItem>(
			WeaponClass,
			SpawnTransform,
			this,
			this,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		if (SpawnedWeapon)
		{
			SpawnedWeapon->SetItemID(ItemID);
			SpawnedWeapon->ApplyAttachmentState(ItemData.AttachmentState);
			SpawnedWeapon->SetCurrentAmmo(ItemData.CurrentAmmo);
			UGameplayStatics::FinishSpawningActor(SpawnedWeapon, SpawnTransform);

			SpawnedWeapon->SetEquippedState();

			if (GetMesh())
			{
				SpawnedWeapon->AttachToComponent(
					GetMesh(),
					FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					WeaponSocketName
				);
				CurrentWeapon = SpawnedWeapon;
				CurrentWeaponItemID = ItemID;
				CurrentOverlayState = Data.WeaponType;
			}

			InventoryComponent->SetEquippedWeapon(SpawnedWeapon);
			InventoryComponent->OnWeaponChanged.Broadcast(true, CurrentWeaponItemID);
			SpawnedWeapon->OnAmmoChanged.Broadcast(SpawnedWeapon->GetCurrentAmmo(), SpawnedWeapon->GetMaxAmmo());
		}
	}
}

FInventoryItem APlayerCharacter::UnequipItemBySlot(ESlotType SlotType)
{
	if (CurrentWeapon)
	{
		FInventoryItem ItemData;
		ItemData.ItemID = CurrentWeaponItemID;
		ItemData.Quantity = 1; // 무기는 수량 1로 간주
		ItemData.AttachmentState = CurrentWeapon->GetAttachmentState();
		ItemData.CurrentAmmo = CurrentWeapon->GetCurrentAmmo();
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
		CurrentWeaponItemID = NAME_None;
		CurrentOverlayState = EWeaponType::WT_None;
		InventoryComponent->SetEquippedWeapon(nullptr);
		InventoryComponent->OnWeaponChanged.Broadcast(false, ItemData.ItemID);
		return ItemData;
	}
	else return FInventoryItem();	
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

	OnStaminaChanged.Broadcast(NewStamina);
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
	OnStaminaChanged.Broadcast(NewValue);
	StatComp->SetCurrentStatValue("Stamina", NewValue);
}