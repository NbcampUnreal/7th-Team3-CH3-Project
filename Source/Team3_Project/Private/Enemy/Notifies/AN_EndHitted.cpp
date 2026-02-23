#include "Enemy/Notifies/AN_EndHitted.h"
#include "Enemy/EnemyCharacter.h"

void UAN_EndHitted::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    AActor* OwnerActor = MeshComp->GetOwner();
    if (!OwnerActor) return;

    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerActor);
    if (!EnemyCharacter) return;

    EnemyCharacter->OnFinishHitted();
}
