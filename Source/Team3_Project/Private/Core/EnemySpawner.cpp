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
	if (MaxSpawnMonster > CurrentSpawnMonster)
	{
		if (!EnemyClass)
		{
			return;
		}

		AEnemyCharacter* SpawnMonster = GetWorld()->SpawnActor<AEnemyCharacter>(
			EnemyClass,
			GetActorLocation(),
			FRotator::ZeroRotator
		);

		//TODO_@Enemy : OnDead 델리게이트를 통해 CurrentSpawnMonster -1 전달해주어야함
		if (IsValid(SpawnMonster))
		{
			++CurrentSpawnMonster;
			SpawnMonster->OnDeadSignature.RemoveDynamic(this, &AEnemySpawner::SetCurrentSpawnEnemy);
			SpawnMonster->OnDeadSignature.AddDynamic(this, &AEnemySpawner::SetCurrentSpawnEnemy);
		}
	}
	else
	{
		return;
	}
}

void AEnemySpawner::SetCurrentSpawnEnemy()
{
	CurrentSpawnMonster--;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}



