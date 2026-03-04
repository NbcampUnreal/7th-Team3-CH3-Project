// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainGameState.h"
#include "Core/EnemySpawner.h"
#include "Core/EventZone.h"
#include "Core/DebugHelper.h"
#include "Core/MainGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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
	
	//OnGameStart();

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
		return;
	}

	if (bIsRunningSpawner)
	{
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
		return;
	}

	if (TWeakObjectPtr<AEnemySpawner>* FoundSpawner1 = EnemySpawners.Find(Id + (Id - 1)))
	{
		if (FoundSpawner1->IsValid())
		{
			FoundSpawner1->Get()->SpawnEnemy(EnemyClass);
		}
	}

	if (TWeakObjectPtr<AEnemySpawner>* FoundSpawner2 = EnemySpawners.Find(2 * Id))
	{
		if (FoundSpawner2->IsValid())
		{
			FoundSpawner2->Get()->SpawnEnemy(EnemyClass);
		}
	}
}
void AMainGameState::WaveStart()
{
	OnWaveStart.Broadcast(TEXT("Wave Start!!"), MaxWaveTime);
	OnWaveFire.Broadcast();
	//PlayerState = PointMode
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
	OnWaveEnd.Broadcast(true, 100);
	OnWaveStop.Broadcast();

	//PlayerState = NormalMode
	GetWorldTimerManager().ClearTimer(WaveStartTimer); // 스폰 타이머 삭제
	EnemySpawnDelegate.Unbind(); // 바인딩 된 함수 해제
	bIsRunningSpawner = false;
	UMainGameInstance::Get(GetWorld())->TotalScore += CurrentScore; // GI에 현재 웨이브에서 얻은 점수 전달

}

void AMainGameState::OnGameStart()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	UGameplayStatics::SetGamePaused(this, true);
	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;

	if (!GameStartWidgetClass || !PC)
	{
		return;
	}

	UUserWidget* GameStartWidget = CreateWidget<UUserWidget>(PC, GameStartWidgetClass);
	if (GameStartWidget)
	{
		GameStartWidget->AddToViewport();
	}
}

void AMainGameState::OnGameOver(bool bIsDeadPlayer)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	UGameplayStatics::SetGamePaused(this, true);
	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;

	if (!GameOverWidgetClass || !PC)
	{
		return;
	}

	UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
	if (GameOverWidget)
	{
		GameOverWidget->AddToViewport();
	}
}

void AMainGameState::OnOpenMenu()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	UGameplayStatics::SetGamePaused(this, true);
	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;

	if (!MainMenuWidgetClass || !PC)
	{
		return;
	}

	UUserWidget* MainMenuWidget = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
	if (MainMenuWidget)
	{
		MainMenuWidget->AddToViewport();
	}
}
