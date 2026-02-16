// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WeaponItem.h"
#include "Kismet/GameplayStatics.h"
#include "Item/BaseProjectile.h"
#include "DrawDebugHelpers.h"
#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#define ECC_Weapon ECC_GameTraceChannel1

AWeaponItem::AWeaponItem()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	ScopeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScopeMesh"));
	ScopeMesh->SetupAttachment(WeaponMesh);

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	BarrelMesh->SetupAttachment(WeaponMesh);

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
	MagazineMesh->SetupAttachment(WeaponMesh);

	UnderbarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnderbarrelMesh"));
	UnderbarrelMesh->SetupAttachment(WeaponMesh);

	StockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StockMesh"));
	StockMesh->SetupAttachment(WeaponMesh);

	MaxAmmo = 30;
	CurrentAmmo = 30;
	TimeBetweenShots = 0.1f;
	bIsAutomatic = true;
	WeaponRange = 5000.0f;
	Pellets = 1;
	BaseDamage = 20.0f;
	SpreadAngle = 1.0f;
	bIsProjectile = false;
	LastFireTime = 0.0f;
	CurrentRecoilPitch = 0.0f;

	// 원래 값 저장
	OriginalDamage = BaseDamage;
	OriginalSpread = SpreadAngle;
	OriginalRecoil = CurrentRecoil;
	OriginalMaxAmmo = MaxAmmo;
	OriginalRange = WeaponRange;
}

void AWeaponItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//반동 회복
	if (CurrentRecoilPitch > 0.0f && !GetWorldTimerManager().IsTimerActive(AutoFireTimerHandle))
	{
		AActor* OwnerActor = GetOwner();
		if (!OwnerActor)
		{
			return;
		}
		APawn* OwnerPawn = Cast<APawn>(OwnerActor);
		if (!OwnerPawn)
		{
			return;
		}
		APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
		if (!PlayerController)
		{
			return;
		}

		float OldRecoil = CurrentRecoilPitch;
		float NewRecoil = FMath::FInterpTo(CurrentRecoilPitch, 0.0f, DeltaTime, RecoilRecoveryRate);

		float RecoveryAmount = OldRecoil - NewRecoil;

		PlayerController->AddPitchInput(RecoveryAmount);

		CurrentRecoilPitch = NewRecoil;

		if (CurrentRecoilPitch <= KINDA_SMALL_NUMBER)
		{
			CurrentRecoilPitch = 0.0f;
		}
	}
}









void AWeaponItem::StartFire()
{
	if (bIsReloading)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.5f,
				FColor::Yellow,
				FString::Printf(TEXT("Cannot fire while reloading!"))
			);
		}
		return;
	}

	// 기본 딜레이 계산
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	// 타이머 설정
	GetWorldTimerManager().SetTimer(
		AutoFireTimerHandle,
		this,
		&AWeaponItem::FireWeapon,
		TimeBetweenShots,
		bIsAutomatic,
		FirstDelay
	);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Green,
			FString::Printf(TEXT("Start Fire Triggered"))
		);
	}
}

void AWeaponItem::StopFire()
{
	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Red,
			FString::Printf(TEXT("Stop Fire Triggered"))
		);
	}
}





void AWeaponItem::FireWeapon()
{
	//발사 가능 여부 확인
	if (bIsReloading || CurrentAmmo <= 0)
	{
		StopFire();
		return;
	}
	//탄	약 감소 및 발사 시간 기록
	CurrentAmmo--;
	LastFireTime = GetWorld()->TimeSeconds;

	//발사 방식에 따른 처리
	if (bIsProjectile)
	{
		FireProjectile();
	}
	else
	{
		FireHitScan();
	}

	//반동 적용
	ApplyRecoil();

	//디버그 메시지 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Red,
			FString::Printf(TEXT("Fired! Current Ammo: %d"), CurrentAmmo)
		);
	}
	//발사 이펙트 및 사운드 재생 등 추가 구현 가능
}



void AWeaponItem::ReloadWeapon()
{
	if (bIsReloading)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.5f,
				FColor::Yellow,
				FString::Printf(TEXT("Already reloading!"))
			);
		}
		return;
	}

	if (CurrentAmmo >= MaxAmmo)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.5f,
				FColor::Yellow,
				FString::Printf(TEXT("Ammo is already full!"))
			);
		}
		return;
	}

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon has no owner"));
		return;
	}

	UInventoryComponent* InventoryComp = OwnerActor->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent not found on OwnerActor"));
		return;
	}

	int32 AmmoNeeded = MaxAmmo - CurrentAmmo;

	int32 AmmoAvailable = InventoryComp->GetItemQuantity(AmmoItemID);

	if (AmmoAvailable <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ammo available in inventory"));
		//총알이 부족한 경우 처리 추가 가능
		return;
	}

	bIsReloading = true;
	StopFire();

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&AWeaponItem::FinishReloading,
		ReloadTime,
		false
	);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Green,
			FString::Printf(TEXT("Reloading started"))
		);
	}
}

void AWeaponItem::StopReload()
{
	if (!bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not currently reloading"));
		return;
	}

	bIsReloading = false;
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Red,
			FString::Printf(TEXT("Reloading cancelled"))
		);
	}

	//추후 추가 로직 사용 가능(애니메이션 캔슬 등)
}

void AWeaponItem::FinishReloading()
{
	bIsReloading = false;

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon has no owner"));
		return;
	}
	UInventoryComponent* InventoryComp = OwnerActor->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent not found on OwnerActor"));
		return;
	}

	int32 AmmoNeeded = MaxAmmo - CurrentAmmo;
	if (AmmoNeeded <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ammo needed for reload"));
		return;
	}
	int32 AmmoAvailable = InventoryComp->GetItemQuantity(AmmoItemID);
	int32 AmmoToReload = FMath::Min(AmmoNeeded, AmmoAvailable);


	int32 AmountToReload = FMath::Min(AmmoNeeded, AmmoAvailable);

	if (AmountToReload > 0 && InventoryComp->RemoveItem(AmmoItemID, AmountToReload))
	{
		CurrentAmmo += AmountToReload;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.5f,
				FColor::Green,
				FString::Printf(TEXT("Reloaded %d ammo. Current Ammo: %d"), AmountToReload, CurrentAmmo)
			);
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.5f,
				FColor::Cyan,
				FString::Printf(TEXT("Remaining ammo in inventory: %d"), InventoryComp->GetItemQuantity(AmmoItemID))
			);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to reload ammo"));
	}
}









//반환값 : 이전에 장착된 부착물의 ItemID, 없으면 NAME_None
FName AWeaponItem::EquipAttachment(FName AttachmentID)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return NAME_None;
	}

	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return NAME_None;
	}

	FItemData AttachmentData = ItemDataSubsystem->GetItemDataByID(AttachmentID);
	// 유효성 검사
	if (AttachmentData.ItemType != EItemType::IT_Attachment)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemID %s is not an attachment"), *AttachmentID.ToString());
		return NAME_None;
	}

	EAttachmentType SlotType = AttachmentData.AttachmentType;
	FName ReturnedItem = NAME_None;

	// 이미 해당 슬롯에 장착된 부착물이 있는 경우 처리
	if (EquippedAttachments.Contains(SlotType))
	{
		ReturnedItem = EquippedAttachments[SlotType];
		UStaticMeshComponent* TargetMesh = GetAttachmentComponentByType(SlotType);
		if (TargetMesh)
		{
			TargetMesh->SetStaticMesh(nullptr);
			TargetMesh->SetVisibility(false);
			TargetMesh->SetHiddenInGame(true);
			UE_LOG(LogTemp, Log, TEXT("Unequipped attachment %s"), *ReturnedItem.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No valid attachment component for type %d"), (int32)AttachmentData.AttachmentType);
		}
		EquippedAttachments.Remove(SlotType);
	}

	UStaticMeshComponent* TargetMesh = GetAttachmentComponentByType(SlotType);

	if (TargetMesh)
	{
		UStaticMesh* AttachmentMesh = AttachmentData.AttachmentMesh.LoadSynchronous();
		if (AttachmentMesh)
		{
			TargetMesh->SetStaticMesh(AttachmentMesh);
			TargetMesh->SetVisibility(true);
			TargetMesh->SetHiddenInGame(false);
			UE_LOG(LogTemp, Log, TEXT("Equipped attachment %s"), *AttachmentID.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load mesh for attachment %s"), *AttachmentID.ToString());
		}
		EquippedAttachments.Add(SlotType, AttachmentID);
		RecalculateStats();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid attachment component for type %d"), (int32)AttachmentData.AttachmentType);
	}

	return ReturnedItem;
}

void AWeaponItem::UnequipAttachment(FName AttachmentID)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return;
	}

	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}

	FItemData AttachmentData = ItemDataSubsystem->GetItemDataByID(AttachmentID);

	EAttachmentType SlotType = AttachmentData.AttachmentType;



	UStaticMeshComponent* TargetMesh = GetAttachmentComponentByType(AttachmentData.AttachmentType);

	if (TargetMesh)
	{
		TargetMesh->SetStaticMesh(nullptr);
		TargetMesh->SetVisibility(false);
		TargetMesh->SetHiddenInGame(true);
		UE_LOG(LogTemp, Log, TEXT("Unequipped attachment %s"), *AttachmentID.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid attachment component for type %d"), (int32)AttachmentData.AttachmentType);
	}
}



void AWeaponItem::ApplyAttachmentState(const TMap<EAttachmentType, FName>& InAttachments)
{

	EquippedAttachments = InAttachments;

	for (const auto& Pair : EquippedAttachments)
	{
		EAttachmentType Type = Pair.Key;
		FName AttachmentItemID = Pair.Value;

		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
		if (!GameInstance)
		{
			continue;
		}
		UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
		if (!ItemDataSubsystem)
		{
			continue;
		}

		FItemData AttachmentData = ItemDataSubsystem->GetItemDataByID(AttachmentItemID);
		UStaticMeshComponent* TargetMesh = GetAttachmentComponentByType(Type);

		if (TargetMesh)
		{
			UStaticMesh* AttachmentMesh = AttachmentData.AttachmentMesh.LoadSynchronous();
			if (AttachmentMesh)
			{
				TargetMesh->SetStaticMesh(AttachmentMesh);
				TargetMesh->SetVisibility(true);
				TargetMesh->SetHiddenInGame(false);
				UE_LOG(LogTemp, Log, TEXT("Applied attachment %s"), *AttachmentItemID.ToString());
			}
		}
	}
	RecalculateStats();
}













void AWeaponItem::FireHitScan()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(FName("Muzzle"));
	FVector MuzzleForward = WeaponMesh->GetSocketRotation(FName("Muzzle")).Vector();

	for (int32 i = 0; i < Pellets; i++)
	{
		//스프레드 적용
		FRotator SpreadRot = FRotator(
			FMath::RandRange(-SpreadAngle, SpreadAngle),
			FMath::RandRange(-SpreadAngle, SpreadAngle),
			0.0f
		);
		FVector CameraShotDirection = (CameraRotation + SpreadRot).Vector();
		FVector CameraTraceEnd = CameraLocation + (CameraShotDirection * WeaponRange);

		//카메라 기준 라인 트레이스 수행 (실제 총알 판정을 카메라에서 함)
		FHitResult CameraHit;
		FCollisionQueryParams CameraQueryParams;
		CameraQueryParams.AddIgnoredActor(this);
		CameraQueryParams.AddIgnoredActor(OwnerActor);

		bool bCameraHit = GetWorld()->LineTraceSingleByChannel(
			CameraHit,
			CameraLocation,
			CameraTraceEnd,
			ECC_Visibility,
			CameraQueryParams
		);

		FVector DesiredTargetPoint = bCameraHit ? CameraHit.ImpactPoint : CameraTraceEnd;

		FVector DirectionToTarget = (DesiredTargetPoint - MuzzleLocation).GetSafeNormal();

		float DotProduct = FVector::DotProduct(DirectionToTarget, MuzzleForward);

		if (DotProduct < 0.5f) //총구 방향과 카메라 기준 발사 방향이 너무 다르면 총구 기준으로 발사 방향 보정
		{
			DirectionToTarget = MuzzleForward;
			DesiredTargetPoint = MuzzleLocation + (DirectionToTarget * WeaponRange);
			bCameraHit = false; //카메라 기준으로는 명중이지만 총구 기준으로는 명중이 아니므로 피해 적용 안함
		}

		FHitResult MuzzleHit;
		FCollisionQueryParams BulletQueryParams;
		BulletQueryParams.AddIgnoredActor(this);
		BulletQueryParams.AddIgnoredActor(OwnerActor);

		bool bMuzzleHit = GetWorld()->LineTraceSingleByChannel(
			MuzzleHit,
			MuzzleLocation,
			DesiredTargetPoint,
			ECC_Weapon,
			BulletQueryParams
		);

		FVector FinalImpactPoint = DesiredTargetPoint;

		bool bCanApplyDamage = false;
		bool bVisualBlocked = false;

		//총구에 무언가 맞았다면, 그것이 '목표'인지 '방해물'인지 판단하는 로직
		if (bMuzzleHit)
		{
			//거리 계산: 총구에서 목표까지의 거리 vs 총구에서 명중 지점까지의 거리
			float DistanceToTarget = FVector::Dist(MuzzleLocation, DesiredTargetPoint);
			float DistanceToHit = MuzzleHit.Distance;

			AActor* CameraTargetActor = bCameraHit ? CameraHit.GetActor() : nullptr;
			AActor* MuzzleHitActor = MuzzleHit.GetActor();

			bool bHitSameActor = (CameraTargetActor && MuzzleHitActor && CameraTargetActor == MuzzleHitActor);
			//명중 지점이 목표보다 충분히 가까우면(즉, 명중 지점이 목표를 가로막는 방해물이라면) 명중 지점을 최종 임팩트 포인트로 사용
			if (!bHitSameActor && DistanceToHit < DistanceToTarget - 20.0f)
			{
				FinalImpactPoint = MuzzleHit.ImpactPoint;
				bCanApplyDamage = false;
				bVisualBlocked = true;
			}
			else
			{
				//명중 지점이 목표보다 멀거나 거의 같으면(즉, 명중 지점이 목표 뒤에 있거나 목표와 거의 같은 위치라면) 목표 지점을 최종 임팩트 포인트로 사용
				FinalImpactPoint = MuzzleHit.ImpactPoint;
				bCanApplyDamage = true;
			}
		}
		else
		{
			if (bCameraHit)
			{
				bCanApplyDamage = true;
			}
		}

		if (bCanApplyDamage)
		{
			//피해 적용
			AActor* HitActor = bMuzzleHit ? MuzzleHit.GetActor() : (bCameraHit ? CameraHit.GetActor() : nullptr);
			FHitResult& FinalHitResult = bMuzzleHit ? MuzzleHit : CameraHit;

			if (HitActor)
			{
				UGameplayStatics::ApplyPointDamage(
					HitActor,
					BaseDamage,
					DirectionToTarget,
					FinalHitResult,
					GetInstigatorController(),
					this,
					nullptr
				);
			}
			//디버그용 점 그리기
			DrawDebugPoint(
				GetWorld(),
				FinalImpactPoint,
				10.0f,
				FColor::Red,
				false,
				2.0f
			);
			//임팩트 이펙트 재생 등 추가 구현 가능
		}
		//디버그용 라인 그리기
		DrawDebugLine(
			GetWorld(),
			MuzzleLocation,
			FinalImpactPoint,
			bVisualBlocked ? FColor::Yellow : (bCanApplyDamage ? FColor::Green : FColor::Red),
			false,
			2.0f,
			0,
			1.0f
		);
	}
}



void AWeaponItem::FireProjectile()
{
	if (ProjectileClass)
	{
		AActor* OwnerActor = GetOwner();
		if (!OwnerActor)
		{
			return;
		}

		APawn* OwnerPawn = Cast<APawn>(OwnerActor);
		if (!OwnerPawn)
		{
			return;
		}

		APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
		if (!PC)
		{
			return;
		}

		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector CameraTraceEnd = CameraLocation + (CameraRotation.Vector() * WeaponRange);

		FCollisionQueryParams CameraQueryParams;
		CameraQueryParams.AddIgnoredActor(this);
		CameraQueryParams.AddIgnoredActor(OwnerActor);

		FHitResult CameraHit;
		FVector TargetPoint = CameraTraceEnd;
		if (GetWorld()->LineTraceSingleByChannel(
			CameraHit,
			CameraLocation,
			CameraTraceEnd,
			ECC_Visibility,
			CameraQueryParams
		))
		{
			TargetPoint = CameraHit.ImpactPoint;
		}

		FVector Start = WeaponMesh->GetSocketLocation(FName("Muzzle"));
		FVector MuzzleDirection = (TargetPoint - Start).GetSafeNormal();
		FRotator SpawnRotation = MuzzleDirection.Rotation();

		FTransform SpawnTransform(SpawnRotation, Start);

		ABaseProjectile* NewProjectile = GetWorld()->SpawnActorDeferred<ABaseProjectile>(
			ProjectileClass,
			SpawnTransform,
			this,
			GetInstigator(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		if (NewProjectile)
		{
			NewProjectile->Damage = this->BaseDamage;
			UGameplayStatics::FinishSpawningActor(NewProjectile, SpawnTransform);
		}
	}
}

void AWeaponItem::ApplyRecoil()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	//임시 반동 구현. 추후 개선 필요
	float VerticalRecoil = FMath::RandRange(-CurrentRecoil * 0.8f, -CurrentRecoil * 1.2f);
	float HorizontalRecoil = FMath::RandRange(-CurrentRecoil * 0.5f, CurrentRecoil * 0.5f);

	PC->AddPitchInput(VerticalRecoil);
	PC->AddYawInput(HorizontalRecoil);

	CurrentRecoilPitch += FMath::Abs(VerticalRecoil);
}







UStaticMeshComponent* AWeaponItem::GetAttachmentComponentByType(EAttachmentType Type) const
{
	switch (Type)
	{
	case EAttachmentType::AT_Scope:
		return ScopeMesh;
	case EAttachmentType::AT_Barrel:
		return BarrelMesh;
	case EAttachmentType::AT_Magazine:
		return MagazineMesh;
	case EAttachmentType::AT_Underbarrel:
		return UnderbarrelMesh;
	case EAttachmentType::AT_Stock:
		return StockMesh;
	case EAttachmentType::AT_None:
		return nullptr;
	default:
		return nullptr;
	}
}



void AWeaponItem::RecalculateStats()
{
	// 원래 값 복원
	BaseDamage = OriginalDamage;
	SpreadAngle = OriginalSpread;
	CurrentRecoil = OriginalRecoil;
	MaxAmmo = OriginalMaxAmmo;
	WeaponRange = OriginalRange;

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance)
	{
		return;
	}
	UItemDataSubsystem* ItemDataSubsystem = GameInstance->GetSubsystem<UItemDataSubsystem>();
	if (!ItemDataSubsystem)
	{
		return;
	}

	for (const auto& Pair : EquippedAttachments)
	{
		EAttachmentType Type = Pair.Key;
		FName AttachmentItemID = Pair.Value;

		FItemData Data = ItemDataSubsystem->GetItemDataByID(AttachmentItemID);

		// 타입에 따른 스탯 보정 적용
		switch (Type)
		{
		case EAttachmentType::AT_Scope:
			WeaponRange += Data.PowerAmount;
			UE_LOG(LogTemp, Log, TEXT("Scope attached, new WeaponRange: %f"), WeaponRange);
			break;
		case EAttachmentType::AT_Barrel:
			SpreadAngle -= Data.PowerAmount;
			UE_LOG(LogTemp, Log, TEXT("Barrel attached, new SpreadAngle: %f"), SpreadAngle);

			if (ItemID.ToString().Contains("Silencer"))
			{
				bIsSilenced = true;
			}
			break;
		case EAttachmentType::AT_Magazine:
			MaxAmmo += FMath::RoundToInt(Data.PowerAmount);
			UE_LOG(LogTemp, Log, TEXT("Magazine attached, new MaxAmmo: %d"), MaxAmmo);
			break;
		case EAttachmentType::AT_Underbarrel:
			CurrentRecoil *= (1.0f - Data.PowerAmount);
			UE_LOG(LogTemp, Log, TEXT("Underbarrel attached, new CurrentRecoil: %f"), CurrentRecoil);
			break;
		case EAttachmentType::AT_Stock:
			BaseDamage += Data.PowerAmount;
			UE_LOG(LogTemp, Log, TEXT("Stock attached, new BaseDamage: %f"), BaseDamage);
			break;
		}
	}
}