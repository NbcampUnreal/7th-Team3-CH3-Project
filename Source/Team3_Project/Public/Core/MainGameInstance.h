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

	//레벨이 전환되어도 캐릭터의 스탯은 유지 데이터 테이블로 바뀔 예정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float MaxPlayerHP;

	//플레이어 공격력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float PlayerAttackDamage;

	//카르마 스탯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	int32 KarmaStat;

	
	//게임종료시점 플레이어가 획득한 총 점수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	int32 TotalScore;

	
	//함수

};
