// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ABaseItem::Interact(AActor* Interactor)
{
}

void ABaseItem::OnConstruction(const FTransform& Transform)
{
}


