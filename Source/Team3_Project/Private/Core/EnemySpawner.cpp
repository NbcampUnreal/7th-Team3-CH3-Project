// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EnemySpawner.h"
#include "Enemy/EnemyCharacter.h"

#include "Core/DebugHelper.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::SpawnEnemy(TSubclassOf<AActor> EnemyClass)
{
	if (EnemyClass == nullptr) return;

	GetWorld()->SpawnActor<AEnemyCharacter>(
		EnemyClass,
		GetActorLocation(),
		FRotator::ZeroRotator
	);

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}



