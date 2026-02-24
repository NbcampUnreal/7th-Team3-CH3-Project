// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CraftingStation.h"

// Sets default values
ACraftingStation::ACraftingStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ACraftingStation::Interact_Implementation(AActor* Interactor)
{
}

bool ACraftingStation::TryCraftItem(AActor* Crafter, FName RecipeID)
{
	return false;
}

