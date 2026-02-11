// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Shared/ItemTypes.h"
#include "WeaponItem.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class ABaseProjectile;

UCLASS(Blueprintable)
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

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	void StopReload();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	virtual FName EquipAttachment(FName AttachmentID);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	virtual void UnequipAttachment(FName AttachmentID);


	const TMap<EAttachmentType, FName>& GetAttachmentState() const { return EquippedAttachments; }

	void ApplyAttachmentState(const TMap<EAttachmentType, FName>& InAttachments);

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


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
	bool bIsSilenced = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
	bool bIsReloading = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	bool bIsProjectile = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	TSubclassOf<ABaseProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	USoundBase* DefaultFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	USoundBase* SilencedFireSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Attachment")
	TMap<EAttachmentType, FName> EquippedAttachments;

	void RecalculateStats();

	float OriginalRecoil;
	float OriginalRange;
	int32 OriginalMaxAmmo;
	float OriginalSpread;
	float OriginalDamage;
	
	void FireHitScan();
	void FireProjectile();

	FTimerHandle ReloadTimerHandle;
	void FinishReloading();

private:
	UStaticMeshComponent* GetAttachmentComponentByType(EAttachmentType Type) const;

	FTimerHandle AutoFireTimerHandle;

	float LastFireTime = 0.0f;
};
