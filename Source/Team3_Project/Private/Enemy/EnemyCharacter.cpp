#include "Enemy/EnemyCharacter.h"
#include "Shared/Component/StatComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/Controllers/EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Player/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/MainGameInstance.h"
#include "Core/ItemDataSubsystem.h"
#include "Item/BaseItem.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetForAIMove();

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));

	WeaponCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponCollision"));
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	LeftAttackCoolTime = 0.f;
	bIsAttacking = false;
	bIsMovable = true;
	bIsForWave = false;
	bIsDead = false;
	bRagdollEnabled = false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (TypeData)
	{
		if (StatComp)
		{
			float Health = TypeData->BaseHP;
			float Attack = TypeData->BaseAttack;
			float Defense = TypeData->BaseDefense;
			float WhiteKarma = TypeData->WhiteKarma;
			float BlackKarma = TypeData->BlackKarma;
			StatComp->InitializeStat(FName("Health"), Health, 0.f, Health);
			StatComp->InitializeStat(FName("Attack"), Attack, 0.f, 200.f);
			StatComp->InitializeStat(FName("Defence"), Defense, 0.f, 200.f);
			StatComp->InitializeStat(FName("WhiteKarma"), WhiteKarma, 0.f, 200.f);
			StatComp->InitializeStat(FName("BlackKarma"), BlackKarma, 0.f, 200.f);
		}
		
		if (WeaponCollision)
		{
			FName SocketName = TypeData->SocketName;
			float WeaponRadius = TypeData->WeaponRadius;
			ensureMsgf(FMath::IsFinite(WeaponRadius), TEXT("WeaponRadius is not finite"));
			ensureMsgf(WeaponRadius >= 0.f, TEXT("WeaponRadius is negative: %f"), WeaponRadius);

			WeaponCollision->SetSphereRadius(WeaponRadius);
			WeaponCollision->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				SocketName
			);

			WeaponCollision->OnComponentBeginOverlap.AddDynamic(
				this,
				&AEnemyCharacter::OnWeaponBeginOverlap
			);
		}
	}

	ActiveMove();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LeftAttackCoolTime > 0.f)
	{
		LeftAttackCoolTime = LeftAttackCoolTime - DeltaTime;
		if (LeftAttackCoolTime < 0.f) LeftAttackCoolTime = 0.f;
	}

#if WITH_EDITOR
	if (WeaponCollision && WeaponCollision->IsCollisionEnabled())
	{
		DrawDebugSphere(
			GetWorld(),
			WeaponCollision->GetComponentLocation(),
			WeaponCollision->GetScaledSphereRadius(),
			12,
			FColor::Red,
			false,
			-1.f,
			0,
			2.f
		);
	}
#endif
}

bool AEnemyCharacter::DropItem()
{
	// 데이터 세팅 안 된 경우
	if (!TypeData) return false;

	// DropTable이 비어있는 경우
	const TArray<FEnemyDropItem> DropTable = GetDropItemTable();
	if (DropTable.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("DropTable is empty!"));
		return false;
	}
	// Drop 확률 체크
	float Rand = FMath::RandRange(0, 100);
	if (Rand > GetItemDropRatio())
	{
		UE_LOG(LogTemp, Error, TEXT("Under DropRatio!"));
		return false;
	}
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is missing!"));
		return false;
	}

	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDataSubsystem is missing!"));
 		return false;
	}


	FName ItemID;
	int32 CurrentRatio = 0;
	int32 Quantity = 1;
	int32 TargetRatio = FMath::Rand32() % 100;
	for (FEnemyDropItem Row : DropTable)
	{
		CurrentRatio += Row.DropRatio;
		if (CurrentRatio >= TargetRatio)
		{
			ItemID = Row.ItemID;
			Quantity = FMath::RandRange(Row.MinQunatity, Row.MaxQuantity);
		}
	}

	FItemData ItemData = ItemDataSubsystem->GetItemDataByID(ItemID);
	if (ItemData.ItemID != NAME_None)
	{
		UClass* SpawnClass = ItemData.ItemClass.LoadSynchronous();

		if (SpawnClass)
		{
			for (int i = 0; i < Quantity; i++)
			{
				float Degree = (360.f / Quantity) * i;
				float Radian = FMath::DegreesToRadians(Degree);

				FVector SpawnLocation = GetActorLocation()
					+ FVector(FMath::Cos(Radian), FMath::Sin(Radian), 0.f);
				FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

				ABaseItem* DroppedItem = GetWorld()->SpawnActorDeferred<ABaseItem>(
					SpawnClass,
					SpawnTransform,
					nullptr,
					nullptr,
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
				);

				if (DroppedItem)
				{
					DroppedItem->SetItemID(ItemID);
					DroppedItem->SetQuantity(Quantity);
					DroppedItem->FinishSpawning(SpawnTransform);
				}
			}
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to drop ItemID %s with quantity %d enemy name = %s"), *ItemID.ToString(), Quantity, *(GetName().ToString()));
	}

	return false;
}

bool AEnemyCharacter::Attack()
{
	if (IsAttackable())
	{
		LeftAttackCoolTime = GetAttackCoolTime();
		PlayAnimMontage(GetAttackMontage());
		
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			// 이동 속도 감소
			MoveComp->MaxWalkSpeed = GetChaseSpeed() * GetAttackMoveSpeedRatio();
		}

		OnAttackSignature.Broadcast();
		return true;
	}

	return false;
}

bool AEnemyCharacter::SpecialAttack()
{
	if (IsAttackable())
	{
		LeftAttackCoolTime = GetAttackCoolTime();
		PlayAnimMontage(GetSpecialAttackMontage());
		DeactiveMove();

		OnSepcialAttackSignature.Broadcast();
		return true;
	}
	return false;
}

void AEnemyCharacter::OnFinishAttack()
{
	bIsAttacking = false;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		// 이동 속도 감소
		MoveComp->MaxWalkSpeed = GetChaseSpeed();
	}

	OnFinishAttackSignature.Broadcast();
}

void AEnemyCharacter::OnFinishSpecialAttack()
{
	bIsAttacking = false;
	OnFinishSpecialAttackSignature.Broadcast();
}

void AEnemyCharacter::EnableWeaponCollision()
{
	if (bIsDead) return;
	if (WeaponCollision)
	{
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ResetHitActors();  // 활성화와 동시에 Hit 목록 초기화

		UE_LOG(LogTemp, Log, TEXT("[Combat] Weapon collision enabled"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] WeaponCollision is null"));
	}
}

void AEnemyCharacter::DisableWeaponCollision()
{
	if (WeaponCollision)
	{
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		UE_LOG(LogTemp, Log, TEXT("[Combat] Weapon collision disabled"));
	}
}

void AEnemyCharacter::ResetHitActors()
{
	HitActorsThisAttack.Empty();
	UE_LOG(LogTemp, Log, TEXT("[Combat] Hit actors reset"));
}

bool AEnemyCharacter::ExecuteSpecialAttackByID(FName AttackID, AActor* TargetActor)
{
	if (!SpecialAttackData)
	{
		UE_LOG(LogTemp, Error, TEXT("[SpecialAttack] No SpecialAttackData"));
		return false;
	}

	// ========================================
	// ⭐ ID로 공격 찾기
	// ========================================
	USpecialAttackBase* Attack = SpecialAttackData->GetAttackByID(AttackID);

	if (!Attack)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpecialAttack] Attack ID '%s' not found"), *AttackID.ToString());
		return false;
	}

	if (!Attack->CanExecute(this, TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpecialAttack] Cannot execute '%s'"), *AttackID.ToString());
		return false;
	}

	// ========================================
	// 몽타주 재생
	// ========================================
	if (UAnimMontage* Montage = Attack->GetMontage())
	{
		PlayAnimMontage(Montage);
	}

	// 이동 제어
	if (!Attack->CanMoveWhileAttacking())
	{
		DeactiveMove();
	}

	// ========================================
	// ⭐ 공격 실행
	// ========================================
	Attack->Execute(this, TargetActor);

	OnSepcialAttackSignature.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("[SpecialAttack] Executed: %s"), *AttackID.ToString());

	return true;
}

void AEnemyCharacter::TriggerSpecialAttackEffect()
{
	if (!CurrentSpecialAttack)
	{
		UE_LOG(LogTemp, Error, TEXT("[SpecialAttack] No CurrentSpecialAttack!"));
		return;
	}

	// ========================================
	// ⭐ 실제 공격 실행
	// ========================================
	CurrentSpecialAttack->Execute(this, CurrentTargetActor);

	UE_LOG(LogTemp, Log, TEXT("[SpecialAttack] Effect triggered: %s"),
		*CurrentSpecialAttack->GetAttackID().ToString());
}

void AEnemyCharacter::OnHitted()
{
	StopAnimMontage();
	OnHittedSignature.Broadcast();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = GetPatrolSpeed() * GetHittedMoveSpeedRatio();
	}
	if (UAnimMontage* Montage = GetHittedMontage())
	{
		PlayAnimMontage(Montage);
		// DeactiveMove();
		UE_LOG(LogTemp, Warning, TEXT("%s Hitted!"), *(GetActorLabel()));
	}
	else
	{
		OnFinishHitted();
	}
}

void AEnemyCharacter::OnFinishHitted()
{
	ActiveMove();
	OnFinishHittedSignature.Broadcast();
}

void AEnemyCharacter::OnDead(bool bShouldPlayMontage)
{
	bIsDead = true;
	StopAnimMontage();
	DeactiveMove();
	OnDeadSignature.Broadcast();

	// KillCountUp
	UMainGameInstance* MainGameInstance = UMainGameInstance::Get(GetWorld());
	if (MainGameInstance)
	{
		// Kill Count 중가 - 아직 구현 안 됨.
	}

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	if (!bShouldPlayMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnDead] %s - Skipping montage, instant ragdoll"), *GetActorLabel())
		OnFinishDead();
		return;
	}

	if (UAnimMontage* Montage = GetDeadMontage())
	{
		PlayAnimMontage(Montage);
	}
	else
	{
		OnFinishDead();
	}
}

void AEnemyCharacter::OnFinishDead()
{
	EnableRagdoll();
	DisableWeaponCollision();
	SetLifeSpan(5.f);
	UE_LOG(LogTemp, Warning, TEXT("Pause=%d Sim=%d"),
    GetMesh()->bPauseAnims,
    GetMesh()->IsSimulatingPhysics());

	// 사망 처리 완료 후 Item Drop
	DropItem();
	OnFinishDeadSignature.Broadcast();
}

float AEnemyCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInsitagtor,
	AActor* DamageCauser
)
{
	if (bIsDead)
	{
		UE_LOG(LogTemp, Log, TEXT("[TakeDamage] %s is already dead"), *GetActorLabel());
		// if (!bRagdollEnabled) EnableRagdoll();
		return DamageAmount;
	}

	// 데미지 처리
	ApplyDamageToStat(DamageAmount);

	// Hitted or Dead 상태 전환
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return DamageAmount;

	if (StatComp->GetBaseStatValue(FName("Health")) <= 0.f)
	{
		TSubclassOf<UDamageType> DamageTypeClass = DamageEvent.DamageTypeClass;
		bool bShouldPlayMontage = !(TypeData->MontageSkipDamageTypes.Contains(DamageTypeClass));

		UE_LOG(LogTemp, Error, TEXT("[TakeDamage] %s DEAD! DamageType: %s"),
			*GetActorLabel(), *GetNameSafe(DamageTypeClass));
		
		OnDead(bShouldPlayMontage);
	}
	else
	{
		OnHitted();
	}

	return DamageAmount;
}

FName AEnemyCharacter::GetName() const
{
	return EnemyName;
}
float AEnemyCharacter::GetCurrentHealth() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("Health"));
}

float AEnemyCharacter::GetMaxHealth() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetMaxStatValue(TEXT("Health"));
}

float AEnemyCharacter::GetAttack() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("Attack"));
}

float AEnemyCharacter::GetDefence() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("Defence"));
}

float AEnemyCharacter::GetWhiteKarma() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("WhiteKarma"));
}

float AEnemyCharacter::GetBlackKarma() const
{
	if (StatComp == nullptr) return 0.f;

	return StatComp->GetCurrentStatValue(TEXT("BlackKarma"));
}

bool AEnemyCharacter::IsAttackable()
{
	if (!FMath::IsNearlyZero(LeftAttackCoolTime)) return false;
	
	return !bIsAttacking;
}
bool AEnemyCharacter::IsMovable() const
{
	return bIsMovable;
}

bool AEnemyCharacter::IsForWave() const
{
	return bIsForWave;
}

bool AEnemyCharacter::IsDead() const
{
	return bIsDead;
}

bool AEnemyCharacter::IsRagdollEnabled() const
{
	return bRagdollEnabled;
}

void AEnemyCharacter::ApplyWaveFlag(bool bInWave)
{
	bIsForWave = bInWave;

	OnWaveModeSignature.Broadcast(bIsForWave);
}

void AEnemyCharacter::ActiveMove()
{
	bIsMovable = true;
	OnMovableChangedSignature.Broadcast(bIsMovable);
}

void AEnemyCharacter::DeactiveMove()
{
	bIsMovable = false;
	OnMovableChangedSignature.Broadcast(bIsMovable);
}

UAnimMontage* AEnemyCharacter::GetAttackMontage() const
{
	return TypeData ? TypeData->AttackMontage : nullptr;
}

UAnimMontage* AEnemyCharacter::GetSpecialAttackMontage() const
{
	return nullptr;
}

UAnimMontage* AEnemyCharacter::GetHittedMontage() const
{
	return TypeData ? TypeData->HittedMontage : nullptr;
}

UAnimMontage* AEnemyCharacter::GetDeadMontage() const
{
	return TypeData ? TypeData->DeadMontage : nullptr;
}

float AEnemyCharacter::GetAttackCoolTime() const
{
	return TypeData ? TypeData->AttackCoolTime : 5.f;
}

float AEnemyCharacter::GetPatrolSpeed() const
{
	return TypeData ? TypeData->PatrolMaxSpeed : 150.f;
}

float AEnemyCharacter::GetPatrolRadius() const
{
	return TypeData ? TypeData->PatrolRadius : 150.f;
}

float AEnemyCharacter::GetChaseSpeed() const
{
	return TypeData ? TypeData->ChaseMaxSpeed : 600.f;
}

float AEnemyCharacter::GetAttackMoveSpeedRatio() const
{
	return TypeData ? TypeData->AttackMoveSpeedRatio : 0.7f;
}

float AEnemyCharacter::GetHittedMoveSpeedRatio() const
{
	return TypeData ? TypeData->HittedMoveSpeedRatio : 0.1f;
}

float AEnemyCharacter::GetItemDropRatio() const
{
	return TypeData ? TypeData->ItemDropRatio : -1.f;
}

TArray<FEnemyDropItem> AEnemyCharacter::GetDropItemTable() const
{
	return TypeData ? TypeData->DropTable : TArray<FEnemyDropItem>();
}

void AEnemyCharacter::ApplyDamageToStat(float DamageAmount)
{
	float CurrentHealth = StatComp->GetBaseStatValue(FName("Health"));
	float Defence = StatComp->GetBaseStatValue(FName("Defence"));
	float Damage = FMath::Clamp(DamageAmount - Defence, 1, DamageAmount);

	StatComp->SetBaseStatValue(FName("Health"), CurrentHealth - Damage);
	UE_LOG(LogTemp, Warning, TEXT("Get %f damage! Defence : %f, Take ActualDamage : %f | CurrentHealth : %f"), DamageAmount, Defence, Damage, StatComp->GetCurrentStatValue(FName("Health")));
	
	OnHealthChangedSignature.Broadcast(
		StatComp->GetCurrentStatValue(FName("Health")),
		StatComp->GetMaxStatValue(FName("Health")));
}

void AEnemyCharacter::EnableRagdoll()
{
	if (!GetMesh()) return;

	bRagdollEnabled = true;
	
	// 캐릭터 무브먼트 즉시 중지
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false); // 무브먼트 틱 종료
	}

	// 캡슐 콜리전 제거
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	// 메쉬 설정
	GetMesh()->bPauseAnims = true;
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	
	// 메쉬를 캡슐로부터 분리 (메쉬가 자유롭게 날아가게 함)
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->SetAllBodiesPhysicsBlendWeight(1.0f);
}

void AEnemyCharacter::SetForAIMove()
{
	bUseControllerRotationYaw = false;  // AI가 직접 회전하지 않으므로 false로 설정해야 함.
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = true; // 이동 방향으로 회전
	}
}

void AEnemyCharacter::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsDead) return;
	if (!OtherActor || OtherActor == this) return;

	// 중복 타격 방지
	if (HitActorsThisAttack.Contains(OtherActor))
	{
		return; // 이미 맞춤
	}

	// Team3_ProjectCharacter만 공격 (필요시 수정)
	if (!OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		return;
	}

	// 데미지 적용
	float Damage = StatComp->GetCurrentStatValue(FName("Attack"));

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);

	// 타격 목록에 추가
	HitActorsThisAttack.Add(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("[Combat] Hit: %s (Damage: %.1f)"),
		*OtherActor->GetName(), Damage);
}
