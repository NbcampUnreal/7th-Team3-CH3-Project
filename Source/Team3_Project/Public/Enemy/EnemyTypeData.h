#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyTypeData.generated.h"

UCLASS()
class TEAM3_PROJECT_API UEnemyTypeData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> SpecialAttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> HittedMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> DeadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float BaseHP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float BaseAttack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float BaseDefense;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|CoolTime")
	float AttackCoolTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|CoolTime")
	float SpecialAttackCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	float PatrolMaxSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	float PatrolRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	float ChaseMaxSpeed;
};
