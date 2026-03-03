// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Shared/ItemTypes.h"
#include "WeaponItem.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class ABaseProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, MaxAmmo);

UCLASS(Blueprintable)
class TEAM3_PROJECT_API AWeaponItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AWeaponItem();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

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

	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	const TMap<EAttachmentType, FName>& GetAttachmentState() const { return EquippedAttachments; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	void ApplyAttachmentState(const TMap<EAttachmentType, FName>& InAttachments);

	void ResetRecoil() { CurrentRecoilPitch = 0.0f; }

	virtual void Interact_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	void InitializeDroppedWeapon(const TMap<EAttachmentType, FName>& SavedAttachments);

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Ammo")
	FOnAmmoChanged OnAmmoChanged;

	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	void SetEquippedState();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Get")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Get")
	int32 GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Set")
	void SetCurrentAmmo(int32 NewAmmo) { 
		CurrentAmmo = FMath::Clamp(NewAmmo, 0, MaxAmmo); 
		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float RecoilRecoveryRate = 10.0f;


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

	UPROPERTY(EditAnywhere, Category = "Weapon|Effects")
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Weapon|Effects")
	class UParticleSystem* MuzzleFlashFX;

	UPROPERTY(EditAnywhere, Category = "Weapon|Effects")
	class UParticleSystem* DefaultImpactFX;

	UPROPERTY(EditAnywhere, Category = "Weapon|Effects")
	FName MuzzleSocketName = FName("Muzzle");

	// 재질별 피격 이펙트 맵 (Key: 태그이름, Value: 파티클)
	UPROPERTY(EditAnywhere, Category = "Weapon|Effects")
	TMap<FName, UParticleSystem*> ImpactEffectMap;

	UPROPERTY(EditAnywhere, Category = "Weapon|Effects")
	FVector MuzzleFXScale = FVector(0.03f);

	UPROPERTY(EditAnywhere, Category = "Weapon|Effects")
	FVector ImpactFXScale = FVector(0.4f);

	UPROPERTY(EditAnywhere, Category = "Weapon|Animation")
	class UAnimMontage* ReloadMontage;

	void RecalculateStats();

	float OriginalRecoil;
	float OriginalRange;
	int32 OriginalMaxAmmo;
	float OriginalSpread;
	float OriginalDamage;

	float CurrentRecoilPitch = 0.0f;
	
	void FireHitScan();
	void FireProjectile();
	void ApplyRecoil();

	FTimerHandle ReloadTimerHandle;
	void FinishReloading();

	bool bIsOriginalDataSaved = false;

private:
	UStaticMeshComponent* GetAttachmentComponentByType(EAttachmentType Type) const;

	FTimerHandle AutoFireTimerHandle;

	float LastFireTime = 0.0f;
};
