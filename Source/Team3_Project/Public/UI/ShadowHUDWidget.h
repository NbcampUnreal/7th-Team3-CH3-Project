#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShadowHUDWidget.generated.h"

UENUM(BlueprintType)
enum class EShadowUIState : uint8 
{
    Gameplay,     
    KarmaInventory,
    PauseMenu     
};

UCLASS()
class TEAM3_PROJECT_API UShadowHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shadow|State")
    EShadowUIState CurrentUIState;

public:
    UFUNCTION(BlueprintCallable, Category = "Shadow|Update")
    void UpdateStatBars(float HP, float MaxHP, float Karma, float MaxKarma);

    UFUNCTION(BlueprintCallable, Category = "Shadow|Combat")
    void OnEnemyHit(bool bIsCritical, bool bIsKilled);

    UFUNCTION(BlueprintCallable, Category = "Shadow|Inventory")
    void RefreshInventoryUI();
};
