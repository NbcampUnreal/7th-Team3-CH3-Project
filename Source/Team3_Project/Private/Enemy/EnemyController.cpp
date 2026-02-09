#include "Enemy/EnemyController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Enemy/EnemyCharacter.h"

AEnemyController::AEnemyController()
    :BehaviorTree(nullptr),
    AIPerceptionComponent(nullptr),
    SightConfig(nullptr),
    HearingConfig(nullptr)
{
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerceptionComponent);

    // 시야 감각 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1000.f;          // 시야 범위
    SightConfig->LoseSightRadius = 1500.f;      // 시야에서 벗어나는 범위
    SightConfig->PeripheralVisionAngleDegrees = 90.f;       // 시야각 좌우 x도 = 총 2x
    SightConfig->SetMaxAge(5.f);                            // 시각 기억 시간
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.f;

    // 시각 감지 대상 설정
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;          // 적 감지
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;         // 중립 감지
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;      // 아군 감지
    
    PerceptionComponent->ConfigureSense(*SightConfig);

    // 청각 감각 설정
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 2000.f;           // 가청 범위
    HearingConfig->SetMaxAge(5.f);                  // 소리 기억 시간

    // 청각 감지 대상 설정
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;          // 적 감지
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;         // 중립 감지
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;      // 아군 감지

    PerceptionComponent->ConfigureSense(*HearingConfig);

    // 주 감각을 시야로 설정
    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &AEnemyController::OnTargetPerceptionUpdated
    );

    PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(
        this,
        &AEnemyController::OnTargetPerceptionForgotten
    );
}

void AEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

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

            Enemy->ChangeState(EEnemyState::Chasing);
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
        Enemy->ChangeState(EEnemyState::Idle);
    }
}
