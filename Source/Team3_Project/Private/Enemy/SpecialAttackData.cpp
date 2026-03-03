#include "Enemy/SpecialAttackData.h"

void USpecialAttackData::TickCooldowns(float DeltaTime)
{
    for (USpecialAttackBase* Attack : SpecialAttacks)
    {
        if (Attack)
        {
            Attack->OnTick(DeltaTime);
        }
    }
}

USpecialAttackBase* USpecialAttackData::GetAttackByID(FName AttackID) const
{
    for (USpecialAttackBase* Attack : SpecialAttacks)
    {
        if (Attack && Attack->GetAttackID() == AttackID)
        {
            return Attack;
        }
    }
    return nullptr;
}

USpecialAttackBase* USpecialAttackData::GetAttackForDistance(float Distance) const
{
    for (USpecialAttackBase* Attack : SpecialAttacks)
    {
        if (Attack && Distance >= Attack->GetMinRange() && Distance <= Attack->GetMaxRange())
        {
            return Attack;
        }
    }
    return nullptr;
}

TArray<USpecialAttackBase*> USpecialAttackData::GetAvailableAttacks(class AEnemyCharacter* Owner, AActor* Target) const
{
    TArray<USpecialAttackBase*> Available;

    for (USpecialAttackBase* Attack : SpecialAttacks)
    {
        if (Attack && Attack->CanExecute(Owner, Target))
        {
            Available.Add(Attack);
        }
    }

    return Available;
}

USpecialAttackBase* USpecialAttackData::GetRandomAvailableAttack(class AEnemyCharacter* Owner, AActor* Target) const
{
    TArray<USpecialAttackBase*> Available = GetAvailableAttacks(Owner, Target);

    if (Available.Num() == 0) return nullptr;

    int32 Index = FMath::RandRange(0, Available.Num() - 1);
    return Available[Index];
}

float USpecialAttackData::GetCooldownLeft(FName AttackID) const
{
    USpecialAttackBase* Attack = GetAttackByID(AttackID);
    return Attack ? Attack->GetLeftCooldown() : 0.f;
}

bool USpecialAttackData::IsAttackReady(FName AttackID) const
{
    USpecialAttackBase* Attack = GetAttackByID(AttackID);
    return Attack ? Attack->IsReady() : false;
}
