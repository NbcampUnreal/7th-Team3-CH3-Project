#include "Enemy/Notifies/AN_EndDead.h"
#include "Enemy/EnemyCharacter.h"

void UAN_EndDead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    AActor* OwnerActor = MeshComp->GetOwner();
    if (!OwnerActor) return;

    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerActor);
    if (!EnemyCharacter) return;

    EnemyCharacter->OnFinishDead();
}
