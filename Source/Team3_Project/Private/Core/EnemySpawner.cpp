// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/EnemySpawner.h"
#include "Enemy/EnemyCharacter.h"

#include "Core/DebugHelper.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::SpawnEnemy(TSubclassOf<AEnemyCharacter> EnemyClass)
{
	if (!EnemyClass)
	{
		Debug::Print(TEXT("Class is not Found"));
		return;
	}

	Debug::Print(EnemyClass->GetName());
	Debug::Print(FString::FromInt(Id) + TEXT("EnemySpawner 작동중"), FColor::Blue);
	
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



