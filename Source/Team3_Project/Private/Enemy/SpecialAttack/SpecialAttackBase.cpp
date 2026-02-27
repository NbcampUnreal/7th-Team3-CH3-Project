#include "Enemy/SpecialAttack/SpecialAttackBase.h"
#include "Enemy/EnemyCharacter.h"

void USpecialAttackBase::Execute_Implementation(AEnemyCharacter* Owner, AActor* TargetActor)
{
    // 기본 구현: 로그만
    UE_LOG(LogTemp, Warning, TEXT("[SpecialAttack] %s executed (override me!)"), *AttackID.ToString());
}

bool USpecialAttackBase::CanExecute_Implementation(AEnemyCharacter* Owner, AActor* TargetActor) const
{
    if (!Owner) return false;
    if (Owner->IsDead()) return false;

    // 거리 체크
    if (TargetActor)
    {
        float Distance = FVector::Dist(Owner->GetActorLocation(), TargetActor->GetActorLocation());

        if (Distance < MinRange || Distance > MaxRange)
        {
            return false;
        }
    }

    return true;
}