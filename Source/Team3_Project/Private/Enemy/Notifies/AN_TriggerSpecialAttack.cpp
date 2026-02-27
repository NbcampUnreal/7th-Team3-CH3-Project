#include "Enemy/Notifies/AN_TriggerSpecialAttack.h"
#include "Enemy/EnemyCharacter.h"

void UAN_TriggerSpecialAttack::Notify(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp || !MeshComp->GetOwner()) return;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(MeshComp->GetOwner());
    if (!Enemy) return;

    // ⭐ AttackID 전달 불필요! (EnemyCharacter가 알고 있음)
    Enemy->TriggerSpecialAttackEffect();
}