#pragma once

#include "CoreMinimal.h"
#include "Item/BaseProjectile.h"
#include "EnemyProjectile.generated.h"

/**
 * Enemy 전용 발사체
 * - Player만 공격
 * - Enemy의 Instigator 정보 유지
 */
UCLASS()
class TEAM3_PROJECT_API AEnemyProjectile : public ABaseProjectile
{
    GENERATED_BODY()

public:
    AEnemyProjectile();

    void SetHoming(bool IsHoming, AActor* TargetActor, float InHomingAcceleration);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // (부모 클래스의 수정을 허가받지 못했기에) 덮어쓰기
    virtual void OnHit(
        UPrimitiveComponent* HitComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit
    );

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    TSubclassOf<UDamageType> DamageType;

    // 유도 미사일 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Homing")
    bool bIsHoming = false;

    // 유도 타겟
    UPROPERTY(BlueprintReadWrite, Category = "Projectile|Homing")
    AActor* HomingTarget = nullptr;

    // 유도 가속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Homing")
    float HomingAcceleration = 2000.f;
};