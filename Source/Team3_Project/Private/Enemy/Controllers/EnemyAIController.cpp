#include "Enemy/Controllers/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Enemy/EnemyCharacter.h"
#include "Components/SplineComponent.h"
#include "Player/PlayerCharacter.h"

AEnemyAIController::AEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = false;

    // AI Perception 설정
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerceptionComponent);

    // 시각 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = SightRange;
    SightConfig->LoseSightRadius = ChaseRange;
    SightConfig->PeripheralVisionAngleDegrees = 90.f;
    SightConfig->SetMaxAge(5.f);
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.f;

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    AIPerceptionComponent->ConfigureSense(*SightConfig);

    // 청각 설정
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 2000.f;
    HearingConfig->SetMaxAge(3.f);

    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;

    AIPerceptionComponent->ConfigureSense(*HearingConfig);

    // 주 감각 설정
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    // 이벤트 바인딩
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this, &AEnemyAIController::OnTargetPerceptionUpdated);

    AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(
        this, &AEnemyAIController::OnTargetPerceptionForgotten);
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);
    if (!Enemy)
    {
        UE_LOG(LogTemp, Error, TEXT("[AI] OnPossess: Not an EnemyCharacter"));
        return;
    }
    Enemy->SetForAIMove();

    // Blackboard 초기화
    InitializeBlackboard();

    // Binding
    Enemy->OnWaveModeSignature.AddDynamic(this, &AEnemyAIController::SetWaveMode);
    Enemy->OnMovableChangedSignature.AddDynamic(this, &AEnemyAIController::SetMovable);
    Enemy->OnHittedSignature.AddDynamic(this, &AEnemyAIController::OnHitted);
    Enemy->OnFinishHittedSignature.AddDynamic(this, &AEnemyAIController::OnFinishHitted);
    Enemy->OnDeadSignature.AddDynamic(this, &AEnemyAIController::OnDead);
    Enemy->OnFinishDeadSignature.AddDynamic(this, &AEnemyAIController::OnFinishDead);
    
    // Behavior Tree 실행
    if (BehaviorTree)
    {
        RunBehaviorTree(BehaviorTree);
        UE_LOG(LogTemp, Display, TEXT("[AI] BehaviorTree started for %s"), *Enemy->GetName().ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[AI] No BehaviorTree assigned to %s"), *Enemy->GetName().ToString());
    }
}

void AEnemyAIController::InitializeBlackboard()
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Error, TEXT("[AI] No Blackboard Component"));
        return;
    }

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
    if (!Enemy) return;

    // Location
    BB->SetValueAsVector(FName("HomeLocation"), Enemy->GetActorLocation());

    // Flags
    bool bHasSpline = Enemy->FindComponentByClass<USplineComponent>() != nullptr;
    BB->SetValueAsBool(FName("bHasSpline"), bHasSpline);
    BB->SetValueAsBool(FName("bIsWaveMode"), Enemy->IsForWave());
    BB->SetValueAsBool(FName("bIsDead"), Enemy->IsDead());
    BB->SetValueAsBool(FName("bIsHitted"), false);
    BB->SetValueAsBool(FName("bIsMovable"), Enemy->IsMovable());

    // Combat
    BB->SetValueAsInt(FName("AttackCount"), 0);
    BB->SetValueAsInt(FName("ConsecutiveAttacks"), 0);
    BB->SetValueAsFloat(FName("LastAttackTime"), -999.f);
    BB->SetValueAsFloat(FName("LastSpecialAttackTime"), -999.f);

    // Patrol
    BB->SetValueAsInt(FName("SplineIndex"), 0);

    UE_LOG(LogTemp, Log, TEXT("[AI] Blackboard initialized (WaveMode: %d, HasSpline: %d)"),
        Enemy->IsForWave(), bHasSpline);
}

void AEnemyAIController::SetWaveMode(bool bEnabled)
{
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsBool(FName("bIsWaveMode"), bEnabled);
        UE_LOG(LogTemp, Log, TEXT("[AI] Wave mode set to: %d"), bEnabled);


        if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
        {
            BB->SetValueAsObject(FName("TargetActor"), PlayerCharacter);
        }
        
    }
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
    if (!PlayerCharacter) return;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
    if (!Enemy) return;

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;

    // ========================================
    // 시각 감지
    // ========================================
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            UE_LOG(LogTemp, Log, TEXT("[AI] %s detected player by sight"), *Enemy->GetName().ToString());

            BB->SetValueAsObject(FName("TargetActor"), PlayerCharacter);
            BB->SetValueAsVector(FName("TargetLocation"), PlayerCharacter->GetActorLocation());
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[AI] %s lost player from sight"), *Enemy->GetName().ToString());
        }
    }
    // ========================================
    // 청각 감지
    // ========================================
    else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            UE_LOG(LogTemp, Log, TEXT("[AI] %s heard noise at: %s"),
                *Enemy->GetName().ToString(), *Stimulus.StimulusLocation.ToString());

            // 타겟 없을 때만 소리 위치로
            if (!BB->GetValueAsObject(FName("TargetActor")))
            {
                BB->SetValueAsVector(FName("TargetLocation"), Stimulus.StimulusLocation);
            }
        }
    }
}

void AEnemyAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
    if (!Enemy) return;

    // Wave 모드는 타겟 유지
    if (Enemy->IsForWave()) return;

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;

    UE_LOG(LogTemp, Log, TEXT("[AI] %s forgot about actor: %s"),
        *Enemy->GetName().ToString(), *Actor->GetName());

    BB->ClearValue(FName("TargetActor"));
}

void AEnemyAIController::OnHitted()
{
    StopMovement();

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;
    BB->SetValueAsBool(FName("bIsHitted"), true);
}

void AEnemyAIController::OnFinishHitted()
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;
    BB->SetValueAsBool(FName("bIsHitted"), false);
}

void AEnemyAIController::OnDead()
{
    StopMovement();

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;
    BB->SetValueAsBool(FName("bIsDead"), true);

    if (UBrainComponent* Brain = GetBrainComponent())
    {
        Brain->StopLogic(TEXT("Dead"));
    }
    ClearFocus(EAIFocusPriority::Gameplay);
}

void AEnemyAIController::OnFinishDead()
{
    StopMovement();
    ClearFocus(EAIFocusPriority::Gameplay);

    if (UBrainComponent* Brain = GetBrainComponent())
    {
        Brain->StopLogic(TEXT("Dead"));
    }
}

void AEnemyAIController::SetMovable(bool IsMovable)
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;
    BB->SetValueAsBool(FName("bIsMovable"), IsMovable);
}
