// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Shared/ItemTypes.h"
#include "WeaponItem.generated.h"


UCLASS()
class TEAM3_PROJECT_API AWeaponItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AWeaponItem();

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
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float CurrentRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float ReloadTime;

private:
	UStaticMeshComponent* GetAttachmentComponentByType(EAttachmentType Type) const;
};
