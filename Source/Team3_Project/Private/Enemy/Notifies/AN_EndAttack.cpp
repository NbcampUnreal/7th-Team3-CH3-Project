#include "Enemy/Notifies/AN_EndAttack.h"
#include "Enemy/EnemyCharacter.h"

void UAN_EndAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    AActor* OwnerActor = MeshComp->GetOwner();
    if (!OwnerActor) return;

    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerActor);
    if (!EnemyCharacter) return;

    EnemyCharacter->OnFinishAttack();
}
