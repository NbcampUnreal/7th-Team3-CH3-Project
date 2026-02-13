// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemyCharacter;

UCLASS()
class TEAM3_PROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

public:
	void SpawnEnemy(TSubclassOf<AEnemyCharacter> EnemyClass);
	
	void InitId(int32 OwnId) { Id = OwnId; }
	int32 GetId() const { return Id; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "ID")
	int32 Id = 0;
};
