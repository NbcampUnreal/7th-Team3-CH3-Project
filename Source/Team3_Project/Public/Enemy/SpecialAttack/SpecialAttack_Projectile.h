#pragma once

#include "CoreMinimal.h"
#include "Enemy/SpecialAttack/SpecialAttackBase.h"
#include "SpecialAttack_Projectile.generated.h"

class AEnemyProjectile;

/**
 * 투사체 발사 공격
 */
UCLASS(Blueprintable)
class TEAM3_PROJECT_API USpecialAttack_Projectile : public USpecialAttackBase
{
    GENERATED_BODY()

public:
    USpecialAttack_Projectile();

    virtual void Execute_Implementation(AEnemyCharacter* Owner, AActor* TargetActor) override;

protected:
    // 투사체 클래스 (BaseProjectile)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<AEnemyProjectile> ProjectileClass;

    // Spawn Socket
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    FName SpawnSocket = FName("hand_r_socket");

    // 발사 속도
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    float ProjectileSpeed = 1000.f;

    // 투사체 수 (다중 발사)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = "1"))
    int32 ProjectileCount = 1;

    // 발사 각도 (다중 발사 시)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    float SpreadAngle = 15.f;

    // 유도 미사일 여부
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    bool bIsHoming = false;

    // 유도 가속도
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    float HomingAcceleration = 2000.f;

    // 생존 시간 (0이면 무한)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    float ProjectileLifeSpan = 10.f;
};