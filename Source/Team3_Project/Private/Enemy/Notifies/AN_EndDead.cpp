// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Notifies/AN_EndDead.h"
#include "Enemy/EnemyCharacter.h"

void UAN_EndDead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(MeshComp->GetOwner());
	if (Enemy)
	{
		Enemy->OnFinishDead();
	}
}
