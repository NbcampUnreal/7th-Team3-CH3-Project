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
#include "Enemy/ChaseState.h"
#include "Enemy/AttackState.h"

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
    ChaseState(nullptr),
    AttackState(nullptr)
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
}

void AEnemyController::BeginPlay()
{
    Super::BeginPlay();

    // State 생성
    IdleState = NewObject<UIdleState>(this);
    IdleState->Init(this);
    ChaseState = NewObject<UChaseState>(this);
    ChaseState->Init(this);
    AttackState = NewObject<UAttackState>(this);
    AttackState->Init(this);

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

    if (ACharacter* PossessedCharacter = Cast<ACharacter>(InPawn))
    {
        PossessedCharacter->bUseControllerRotationYaw = false;
        if (UCharacterMovementComponent* MoveComp = PossessedCharacter->GetCharacterMovement())
        {
            MoveComp->bOrientRotationToMovement = true;
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

void AEnemyController::ChangeState(UStateBase* NewState)
{
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
    FVector CharacterLocation = GetPawn()->GetActorLocation();
    float Dist = (CharacterLocation - TargetLocation).Size();
    if (FMath::IsNearlyZero(Dist))
    {
        TargetLocation = CharacterLocation + FVector(FMath::FRandRange(-100.f, 100.f));
    }

    MoveToLocation(TargetLocation);
}

void AEnemyController::MoveToPlayer()
{
    if (IsValid(TargetActor))
    {
        MoveToActor(TargetActor);
    }
}

bool AEnemyController::IsMoving() const
{
    UCharacterMovementComponent* MovementComp =
        GetPawn()->GetComponentByClass<UCharacterMovementComponent>();
    if (!MovementComp) return false;
    else if (FMath::IsNearlyZero(MovementComp->GetLastUpdateVelocity().Size()))
    {
        return false;
    }

    return true;
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
                BB->SetValueAsVector(FName(""), Stimulus.StimulusLocation);
            }
            TargetLocation = Stimulus.StimulusLocation;
        }
        else
        {
            OnTargetPerceptionForgotten(PlayerCharacter);
        }
    }
}

void AEnemyController::OnTargetPerceptionForgotten(AActor* Actor)
{
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
