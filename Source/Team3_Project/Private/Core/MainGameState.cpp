// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameState.h"
#include "Core/EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Core/DebugHelper.h"
#include "Core/MainGameInstance.h"

AMainGameState::AMainGameState()
{
	MaxWaveTime = 60.f;
	CurrentWaveTime = 0.f;
	MaxSpawnCount = 10;
	CurrentSpawnCount = 0;
	CurrentScore = 0;
}

AMainGameState* AMainGameState::Get(const UWorld* WorldObject)
{
	UWorld* CurrentWorld = WorldObject->GetWorld();
	if (CurrentWorld == nullptr)
	{
		return nullptr;
	}

	AMainGameState* CurrentGameState = Cast<AMainGameState>(CurrentWorld->GetGameState());
	if (CurrentGameState == nullptr)
	{
		return nullptr;
	}

	return CurrentGameState;
}

void AMainGameState::BeginPlay()
{
	Super::BeginPlay();
	FindSpawner();
	OnGameStart();

}

void AMainGameState::FindSpawner()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), Actors);
	if (Actors.IsEmpty())
	{
		Debug::Print(TEXT("Could not Found EnemySpawner Actors"));
		return;
	}

	for (AActor* Values : Actors)
	{
		if (AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(Values))
		{
			EnemySpawners.Add(EnemySpawner);
		}
	}
}

void AMainGameState::SpawnMonster()
{
	if (MaxSpawnCount < CurrentSpawnCount)
	{
		// 몬스터 소환
	}
}
void AMainGameState::WaveStart()
{
	//PlayerState = PointMode

	//웨이브 시작시 타이머 시작
	GetWorldTimerManager().SetTimer(
		WaveStartTimer,
		this,
		&AMainGameState::SpawnMonster,
		0.1f,
		true
	);

	GetWorldTimerManager().SetTimer(
		WaveEndTimer,
		this,
		&AMainGameState::WaveEnd,
		MaxWaveTime,
		false
	);
}
void AMainGameState::WaveEnd()
{
	//웨이브 종료 시 이번웨이브에서 획득한 점수 GameInstance로 전달 몬스터 스폰 종료, 점수모드 비활성화,

	//PlayerState = NormalMode

	GetWorldTimerManager().ClearTimer(WaveStartTimer); // 스폰 타이머 삭제

	UMainGameInstance::Get(GetWorld())->TotalScore += CurrentScore; // GI에 현재 웨이브에서 얻은 점수 전달
	
	//웨이브 종료 전달. 
}

void AMainGameState::OnGameStart()
{
}

void AMainGameState::OnGameOver()
{
}

void AMainGameState::OnOpenMenu()
{
}
