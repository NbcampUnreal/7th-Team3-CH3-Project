#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KarmaDamageText.generated.h"

UCLASS()
class TEAM3_PROJECT_API AKarmaDamageText : public AActor
{
	GENERATED_BODY()
	
public:	
	AKarmaDamageText();

	UFUNCTION(BlueprintCallable, Category = "Shadow|Effect")
	void InitializeDamageText(float DamageAmount, FLinearColor TextColor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadow|Effect")
	float DecayTime;
};
