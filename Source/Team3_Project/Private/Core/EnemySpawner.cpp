// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EnemySpawner.h"
#include "Enemy/EnemyCharacter.h"
#include "Core/DebugHelper.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	MaxSpawnMonster = 10;
	CurrentSpawnMonster = 0;

	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::SpawnEnemy(TSubclassOf<AEnemyCharacter> EnemyClass)
{
	if (!EnemyClass)
	{
		Debug::Print(TEXT("Enemy 클래스를 찾을 수 없습니다."));
		return;
	}

	Debug::Print(EnemyClass->GetName());
	Debug::Print(FString::FromInt(Id) + TEXT("EnemySpawner 작동중"), FColor::Blue);
	
	GetWorld()->SpawnActor<AEnemyCharacter>(
		EnemyClass,
		GetActorLocation(),
		FRotator::ZeroRotator
	);

	/*if (MaxSpawnMonster > CurrentSpawnMonster)
	{
		CurrentSpawnMonster++;
	}
	else
	{
		Debug::Print(TEXT("현재 스폰된 몬스터 수가 최대입니다."));
	}*/
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}



