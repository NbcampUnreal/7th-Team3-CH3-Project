#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AOEIndicator.generated.h"

UCLASS()
class TEAM3_PROJECT_API AAOEIndicator : public AActor
{
    GENERATED_BODY()

public:
    AAOEIndicator();

    // AOE 시작
    UFUNCTION(BlueprintCallable, Category = "AOE")
    void StartAOE(
        AController* InInstigatorController,
        float InBaseDamage,
        float InMaxRadius,
        float InDuration,
        float InGrowthSpeed,
        float InDamageMultiplier
    );

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Overlap 시작
    UFUNCTION()
    void OnAOEBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );


    void UpdateMeshScale();
    void UpdateMaterialOpacity();

protected:
    // 시각 효과용 Mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* AOEMesh;

    // 충돌 감지용 Sphere
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USphereComponent* CollisionSphere;

    // Dynamic Material Instance
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

private:
    // 런타임 데이터
    float CurrentRadius = 0.f;
    float TargetRadius = 500.f;
    float GrowthRate = 250.f;
    float ElapsedTime = 0.f;
    float TotalDuration = 2.f;

    float BaseDamage = 10.f;
    float DamageMultiplier = 1.f;

    AController* InstigatorController = nullptr;

    // 중복 데미지 방지
    TSet<AActor*> DamagedActors;
};