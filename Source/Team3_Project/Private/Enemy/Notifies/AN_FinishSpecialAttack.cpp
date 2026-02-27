#include "Enemy/Notifies/AN_FinishSpecialAttack.h"
#include "Enemy/EnemyCharacter.h"

void UAN_FinishSpecialAttack::Notify(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp || !MeshComp->GetOwner()) return;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(MeshComp->GetOwner());
    if (!Enemy) return;

    Enemy->OnFinishSpecialAttack();
}
