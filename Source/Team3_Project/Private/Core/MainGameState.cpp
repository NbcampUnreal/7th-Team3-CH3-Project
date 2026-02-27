// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameState.h"
#include "Core/EnemySpawner.h"
#include "Core/EventZone.h"
#include "Kismet/GameplayStatics.h"
#include "Core/DebugHelper.h"
#include "Core/MainGameInstance.h"

AMainGameState::AMainGameState()
{
	MaxWaveTime = 10.f;
	CurrentWaveTime = 0.f;
	CurrentScore = 0;
	bPlayerOnBox = false;
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
	
	OnGameStart();

}

void AMainGameState::FindSpawner(const int32 Id, class AEnemySpawner* Spawner)
{
	if (Spawner == nullptr) return;

	EnemySpawners.Add(Id, Spawner);
}

void AMainGameState::FindEventZone(const int32 Id, class AEventZone* EventZone)
{
	if (EventZone == nullptr) return;

	EventZones.Add(Id, EventZone);
}

void AMainGameState::OnTriggerEvent(const int32 Id, bool bIsPlayerIn)
{

	if (Id <= 0)
	{
		Debug::Print(TEXT("올바르지 않은 객체입니다."));
		return;
	}

	if (bIsRunningSpawner)
	{
		Debug::Print(TEXT("스폰 이벤트가 이미 실행중입니다."));
		return;
	}

	bPlayerOnBox = bIsPlayerIn;

	if (bPlayerOnBox)
	{
		EnemySpawnDelegate.BindUObject(this, &AMainGameState::SpawnMonster, Id);
	}
	else
	{
		EnemySpawnDelegate.Unbind();
	}

}

void AMainGameState::SpawnMonster(const int32 Id)
{
	if (Id <= 0)
	{
		Debug::Print(TEXT("올바르지 않은 객체입니다."));
		return;
	}

	if (TWeakObjectPtr<AEnemySpawner>* FoundSpawner1 = EnemySpawners.Find(Id + (Id - 1)))
	{
		if (FoundSpawner1->IsValid())
		{
			Debug::Print(TEXT("SpawnEnemy1 작동"));
			FoundSpawner1->Get()->SpawnEnemy(EnemyClass);
		}
	}

	if (TWeakObjectPtr<AEnemySpawner>* FoundSpawner2 = EnemySpawners.Find(2 * Id))
	{
		if (FoundSpawner2->IsValid())
		{
			Debug::Print(TEXT("SpawnEnemy2 작동"));
			FoundSpawner2->Get()->SpawnEnemy(EnemyClass);
		}
	}
}
void AMainGameState::WaveStart()
{
	OnWaveStart.Broadcast(TEXT("WaveStart!!"), MaxWaveTime);
	OnWaveFire.Broadcast();
	//PlayerState = PointMode
	Debug::Print(TEXT("Wave Start!"));
	bIsRunningSpawner = true;
	//웨이브 시작시 타이머 시작
	GetWorldTimerManager().SetTimer(
		WaveStartTimer,
		EnemySpawnDelegate,
		1.0f,
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
	//웨이브 종료 전달
	OnWaveEnd.Broadcast(100, true);

	//PlayerState = NormalMode
	Debug::Print(TEXT("웨이브 종료!"));
	GetWorldTimerManager().ClearTimer(WaveStartTimer); // 스폰 타이머 삭제
	Debug::Print(TEXT("Delegate UnBind"));
	EnemySpawnDelegate.Unbind(); // 바인딩 된 함수 해제
	bIsRunningSpawner = false;
	//UMainGameInstance::Get(GetWorld())->TotalScore += CurrentScore; // GI에 현재 웨이브에서 얻은 점수 전달
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
