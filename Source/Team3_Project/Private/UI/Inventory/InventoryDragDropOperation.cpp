// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryDragDropOperation.h"

/*
드래그&드롭으로 인벤토리 슬롯 간 이동/장착/퀵슬롯 배정을 처리할 때
드래그 중인 아이템의 정보(ItemID, 출처 슬롯 인덱스/타입)를 담는 Operation 클래스.

실제 드래그 처리 로직은 InventoryItemSlot에서 생성/세팅하며,
드롭 시에는 대상 슬롯(InventoryItemSlot::NativeOnDrop)에서 Operation을 캐스팅해 분기한다.
*/


