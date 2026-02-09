// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Shared/ItemTypes.h"
#include "WeaponItem.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class ABaseProjectile;

UCLASS()
class TEAM3_PROJECT_API AWeaponItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AWeaponItem();

    UFUNCTION(BlueprintCallable, Category = "Weapon|Input")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Input")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	virtual void FireWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	virtual void ReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	virtual void EquipAttachment();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	virtual void UnequipAttachment();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	UStaticMeshComponent* ScopeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	UStaticMeshComponent* BarrelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	UStaticMeshComponent* MagazineMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	UStaticMeshComponent* UnderbarrelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	UStaticMeshComponent* StockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float TimeBetweenShots = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	FName AmmoItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float CurrentRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float WeaponRange = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float BaseDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	int32 Pellets = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float SpreadAngle = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	bool bIsAutomatic = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	bool bIsProjectile = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	TSubclassOf<ABaseProjectile> ProjectileClass;
	
	void FireHitScan();
	void FireProjectile();

private:
	UStaticMeshComponent* GetAttachmentComponentByType(EAttachmentType Type) const;

	FTimerHandle AutoFireTimerHandle;

	float LastFireTime = 0.0f;
};
