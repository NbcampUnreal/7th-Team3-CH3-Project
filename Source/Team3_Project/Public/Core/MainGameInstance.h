// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEAM3_PROJECT_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMainGameInstance();

	static UMainGameInstance* Get(const UWorld* WorldObject);
	
	//변수

	//레벨이 전환되어도 캐릭터의 스탯은 유지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (PlayerStat))
	float MaxPlayerHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (PlayerStat))
	float PlayerAttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (PlayerStat))
	int32 KarmaStat;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (PlayerStat))
	
	//함수

};
