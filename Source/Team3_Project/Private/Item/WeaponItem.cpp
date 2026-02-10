// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WeaponItem.h"
#include "Kismet/GameplayStatics.h"
#include "Item/BaseProjectile.h"
#include "DrawDebugHelpers.h"
#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"


AWeaponItem::AWeaponItem()
{
	PrimaryActorTick.bCanEverTick = false;

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
}









void AWeaponItem::StartFire()
{
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
	if (CurrentAmmo <= 0)
	{
		StopFire();
		return;
	}

	CurrentAmmo--;
	LastFireTime = GetWorld()->TimeSeconds;

	if (bIsProjectile)
	{
		FireProjectile();
	}
	else
	{
		FireHitScan();
	}

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
	if (CurrentAmmo >= MaxAmmo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ammo is already full"));
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

	int32 AmountToReload = FMath::Min(AmmoNeeded, AmmoAvailable);

	if (InventoryComp->RemoveItem(AmmoItemID, AmountToReload))
	{
		CurrentAmmo += AmountToReload;
		UE_LOG(LogTemp, Log, TEXT("Reloaded %d ammo. CurrentAmmo: %d"), AmountToReload, CurrentAmmo);
		UE_LOG(LogTemp, Log, TEXT("Remaining ammo in inventory: %d"), InventoryComp->GetItemQuantity(AmmoItemID));
		//재장전 애니메이션 및 사운드 재생 등 추가 구현 가능
	}
	
}












void AWeaponItem::EquipAttachment(FName AttachmentID)
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
	// 유효성 검사
	if (AttachmentData.ItemType != EItemType::IT_Attachment)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemID %s is not an attachment"), *AttachmentID.ToString());
		return;
	}

	UStaticMeshComponent* TargetMesh = GetAttachmentComponentByType(AttachmentData.AttachmentType);

	if (TargetMesh)
	{
		UStaticMesh* AttachmentMesh = AttachmentData.PickupMesh.LoadSynchronous();
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid attachment component for type %d"), (int32)AttachmentData.AttachmentType);
	}
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













void AWeaponItem::FireHitScan()
{
	//총구 위치와 방향 계산
	FVector Start = WeaponMesh->GetSocketLocation(FName("Muzzle"));
	FVector MuzzleDirection = WeaponMesh->GetSocketRotation(FName("Muzzle")).Vector();
	FVector End = Start + (MuzzleDirection * WeaponRange);

	for (int32 i = 0; i < Pellets; i++)
	{
		//스프레드 적용
		FRotator SpreadRot = FRotator(
			FMath::RandRange(-SpreadAngle, SpreadAngle),
			FMath::RandRange(-SpreadAngle, SpreadAngle),
			0.0f
		);
		FVector SpreadDirection = SpreadRot.RotateVector(MuzzleDirection);
		FVector SpreadEnd = Start + (SpreadDirection * WeaponRange);
		//라인 트레이스 수행
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(GetOwner());
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			SpreadEnd,
			ECC_Visibility,
			QueryParams
		);
		if (bHit)
		{
			//피해 적용
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				UGameplayStatics::ApplyPointDamage(
					HitActor,
					BaseDamage,
					SpreadDirection,
					HitResult,
					GetInstigatorController(),
					this,
					nullptr
				);
			}
			//디버그용 점 그리기
			DrawDebugPoint(
				GetWorld(),
				HitResult.ImpactPoint,
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
			Start,
			bHit ? HitResult.ImpactPoint : SpreadEnd,
			bHit ? FColor::Green : FColor::Red,
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
		FTransform SpawnTransform = WeaponMesh->GetSocketTransform(FName("Muzzle"));

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
