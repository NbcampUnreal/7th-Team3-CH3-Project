#include "Enemy/EnemyController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy/StateBase.h"
#include "Enemy/IdleState.h"
#include "Enemy/PatrolState.h"
#include "Enemy/ChaseState.h"
#include "Enemy/AttackState.h"
#include "Enemy/HittedState.h"
#include "Enemy/DeadState.h"
#include "NavigationSystem.h"
#include "Navigation//PathFollowingComponent.h"

AEnemyController::AEnemyController()
    :BehaviorTree(nullptr),
    AIPerceptionComponent(nullptr),
    SightConfig(nullptr),
    HearingConfig(nullptr),
    TargetActor(nullptr),
    CurrentState(nullptr),
    SightRange(1000.f),
    ChaseRange(1500.f),
    AttackRange(300.f),
    IdleState(nullptr),
    PatrolState(nullptr),
    ChaseState(nullptr),
    AttackState(nullptr),
    HittedState(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;

    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerceptionComponent);

    // 시야 감각 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = SightRange;          // 시야 범위
    SightConfig->LoseSightRadius = ChaseRange;      // 시야에서 벗어나는 범위
    SightConfig->PeripheralVisionAngleDegrees = 90.f;       // 시야각 좌우 x도 = 총 2x, 중앙 기준 반각
    SightConfig->SetMaxAge(5.f);                            // 시각 기억 시간
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.f;

    // 시각 감지 대상 설정
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;          // 적 감지
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;         // 중립 감지
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;      // 아군 감지
    
    AIPerceptionComponent->ConfigureSense(*SightConfig);

    // 청각 감각 설정
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 2000.f;           // 가청 범위
    HearingConfig->SetMaxAge(5.f);                  // 소리 기억 시간

    // 청각 감지 대상 설정
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;          // 적 감지
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;         // 중립 감지
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;      // 아군 감지

    AIPerceptionComponent->ConfigureSense(*HearingConfig);

    // 주 감각을 시야로 설정
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &AEnemyController::OnTargetPerceptionUpdated
    );

    AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(
        this,
        &AEnemyController::OnTargetPerceptionForgotten
    );

    TargetLocation = FVector::ZeroVector;
}

void AEnemyController::BeginPlay()
{
    Super::BeginPlay();

    // State 생성
    IdleState = NewObject<UIdleState>(this);
    IdleState->Init(this);
    PatrolState = NewObject<UPatrolState>(this);
    PatrolState->Init(this);
    ChaseState = NewObject<UChaseState>(this);
    ChaseState->Init(this);
    AttackState = NewObject<UAttackState>(this);
    AttackState->Init(this);
    HittedState = NewObject<UHittedState>(this);
    HittedState->Init(this);
    DeadState = NewObject<UDeadState>(this);
    DeadState->Init(this);

    ChangeState(IdleState);
}

void AEnemyController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState != nullptr)
    {
        CurrentState->TickState();
    }
}

void AEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (AEnemyCharacter* PossessedCharacter = Cast<AEnemyCharacter>(InPawn))
    {
        PossessedCharacter->bUseControllerRotationYaw = false;
        if (UCharacterMovementComponent* MoveComp = PossessedCharacter->GetCharacterMovement())
        {
            MoveComp->bOrientRotationToMovement = true;

        }
        TargetLocation = InPawn->GetActorLocation();

        if (PossessedCharacter->IsForWave())
        {
            TryApplyWaveSetup();
        }
    }

    if (IsValid(BehaviorTree))
    {
        RunBehaviorTree(BehaviorTree);
    }
}

void AEnemyController::UpdateAI()
{
    if (IsValid(BehaviorTree))
    {
        // Todo AI 업데이트 로직 작성
    }
}

void AEnemyController::TryApplyWaveSetup()
{
    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
    if (!EnemyCharacter) return;

    UWorld* World = GetWorld();
    if (!World) return;

    ACharacter* PlayerCharacter = Cast<ACharacter>(World->GetFirstPlayerController()->GetPawn());
    if (!PlayerCharacter) return;

    TargetActor = PlayerCharacter;

    ChangeState(GetChaseState());
}

bool AEnemyController::IsForWave()
{
    if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn()))
    {
        return EnemyCharacter->IsForWave();
    }
    return false;
}

void AEnemyController::ChangeState(UStateBase* NewState)
{
    if (!NewState) return;
    if (CurrentState == DeadState || bStateLocked) return;
    if (CurrentState == NewState) return;

    if (CurrentState)
    {
        CurrentState->ExitState();
    }

    CurrentState = NewState;

    if (CurrentState)
    {
        CurrentState->EnterState();
    }
}

bool AEnemyController::IsPlayerInRange(float Range) const
{
    if (!IsValid(TargetActor)) return false;
    if (GetPawn() == nullptr) return false;

    const float Distance = GetPawn()->GetDistanceTo(TargetActor);

    return Distance <= Range;
}

void AEnemyController::MoveToRandomLocation()
{
    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
    if (!EnemyCharacter) return;

    // float Dist2D = FVector::Dist2D(GetPawn()->GetActorLocation(),
    //  TargetLocation);
    // if (Dist2D >= 50.f) return;

    const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSystem == nullptr)
    {
        return;
    }

    FNavLocation RandomLocation;
    const bool bFound = NavSystem->GetRandomReachablePointInRadius(EnemyCharacter->GetActorLocation(), EnemyCharacter->GetPatrolRadius(), RandomLocation);
    if (bFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("RandomMove! NewLocation is %s"), *(RandomLocation.Location.ToString()));
        MoveToLocation(RandomLocation.Location);
        TargetLocation = RandomLocation.Location;
        DrowFollowingPath(2.f);
    }
}

void AEnemyController::MoveToPlayer()
{
    if (!IsMovable()) return;

    if (IsValid(TargetActor))
    {
        EPathFollowingRequestResult::Type MoveResult = MoveToActor(TargetActor);
        UE_LOG(LogTemp, Warning, TEXT("MoveResult : %s"), *UEnum::GetValueAsString(MoveResult));
    }
}

bool AEnemyController::IsMoving() const
{
    UCharacterMovementComponent* MovementComp =
        GetPawn()->GetComponentByClass<UCharacterMovementComponent>();
    if (!MovementComp) return false;
    else if (FMath::IsNearlyZero(MovementComp->Velocity.Size()))
    {
        return false;
    }
    return true;
}

bool AEnemyController::IsMovable() const
{
    if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn()))
    {
        return EnemyCharacter->IsMovable();
    }

    return false;
}
float AEnemyController::GetPatrolSpeed() const
{
    if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn()))
    {
        return EnemyCharacter->GetPatrolSpeed();
    }

    return false;
}

float AEnemyController::GetChaseSpeed() const
{
    if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn()))
    {
        return EnemyCharacter->GetChaseSpeed();
    }

    return false;
}


void AEnemyController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // PlayerCharacter 클래스로 추후 수정
    ACharacter* PlayerCharacter = Cast<ACharacter>(Actor);
    if (!PlayerCharacter) return;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
    if (!Enemy) return;

    // 시각에 걸린 것인지 확인
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        if (Stimulus.WasSuccessfullySensed()) // 성곡적으로 감지됨
        {
            UE_LOG(LogTemp, Log, TEXT("%s: Player detected by sight!"), *Enemy->GetName().ToString());

            if (UBlackboardComponent* BB = GetBlackboardComponent())
            {
                BB->SetValueAsObject(FName("TargetActor"), PlayerCharacter);
                BB->SetValueAsVector(FName("TargetLocation"), PlayerCharacter->GetActorLocation());
            }

            TargetActor = PlayerCharacter;
            ChangeState(ChaseState);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("%s: Player lost form sight!"), *Enemy->GetName().ToString());
            OnTargetPerceptionForgotten(PlayerCharacter);
        }
    }
    // 청각에 걸린 것인지 확인
    else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        if (Stimulus.WasSuccessfullySensed()) // 성곡적으로 감지됨
        {
            UE_LOG(LogTemp, Log, TEXT("Heard noise at: %s"),
                *Stimulus.StimulusLocation.ToString());

            // 소리 위치로 이동
            if(UBlackboardComponent * BB = GetBlackboardComponent())
            {
                BB->SetValueAsVector(FName("TargetLocation"), Stimulus.StimulusLocation);
            }
            TargetLocation = Stimulus.StimulusLocation;
            // ChangeState(PatrolState);
        }
    }
}

void AEnemyController::OnTargetPerceptionForgotten(AActor* Actor)
{
    if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn()))
    {
        if (EnemyCharacter->IsForWave())
        {
            return;
        }
    }

    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->ClearValue(FName("TargetActor"));
    }

    if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn()))
    {
        ChangeState(IdleState);
    }

    TargetActor = nullptr;
}

void AEnemyController::DrowFollowingPath(float LifeTime)
{
    UPathFollowingComponent* PathComp = GetPathFollowingComponent();
    if (PathComp == nullptr)
    {
        return;
    }

    FNavPathSharedPtr Path = PathComp->GetPath();
    if (!Path.IsValid() || !Path->IsValid())
    {
        return;
    }

    const TArray<FNavPathPoint>& Points = Path->GetPathPoints();
    for (int32 i = 0; i < Points.Num() - 1; ++i)
    {
        DrawDebugLine(
            GetWorld(),
            Points[i].Location,
            Points[i + 1].Location,
            FColor::Green,
            false,
            LifeTime,
            0,
            3.0f
        );

        DrawDebugSphere(
            GetWorld(),
            Points[i].Location,
            15.0f,
            8,
            FColor::Yellow,
            false,
            LifeTime
        );
    }

    // 최종 목적지 표시
    if (Points.Num() > 0)
    {
        DrawDebugSphere(
            GetWorld(),
            Points.Last().Location,
            25.0f,
            12,
            FColor::Red,
            false,
            LifeTime
        );
    }
}
