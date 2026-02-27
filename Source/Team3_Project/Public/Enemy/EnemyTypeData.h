#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Shared/ItemTypes.h"
#include "EnemyTypeData.generated.h"

USTRUCT(BlueprintType)
struct FEnemyDropItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DropRatio = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinQunatity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxQuantity = 1;
};

UCLASS()
class TEAM3_PROJECT_API UEnemyTypeData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> AttackMontage;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float WhiteKarma;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float BlackKarma;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|CoolTime")
	float AttackCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Collision")
	FName SocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Collision")
	float WeaponRadius = 32.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	float PatrolMaxSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	float PatrolRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	float ChaseMaxSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	float AttackMoveSpeedRatio = 0.7f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
	float HittedMoveSpeedRatio = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float ItemDropRatio = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	TArray<FEnemyDropItem> DropTable;

	// 즉시 OnFinishDead가 호출되어야 하는 데미지 타입들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Death")
	TArray<TSubclassOf<UDamageType>> MontageSkipDamageTypes;
};
