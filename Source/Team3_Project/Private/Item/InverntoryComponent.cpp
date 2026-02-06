// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InverntoryComponent.h"

// Sets default values for this component's properties
UInverntoryComponent::UInverntoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UInverntoryComponent::AddItem(FName ItemID, int32 Quantity)
{
	return false;
}

bool UInverntoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	return false;
}

bool UInverntoryComponent::HasItem(FName ItemID, int32 Quantity) const
{
	return false;
}

