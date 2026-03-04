#include "Enemy/SpecialAttack/AOEIndicator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

AAOEIndicator::AAOEIndicator()
{
    PrimaryActorTick.bCanEverTick = true;

    // Root: Collision Sphere
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = CollisionSphere;

    CollisionSphere->SetSphereRadius(1.f);  // 시작 크기
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Visual Mesh (Cylinder 추천)
    AOEMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    AOEMesh->SetupAttachment(RootComponent);
    AOEMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 기본 Cylinder Mesh 설정 (BP에서 교체 가능)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
        TEXT("/Engine/BasicShapes/Cylinder")
    );
    if (CylinderMesh.Succeeded())
    {
        AOEMesh->SetStaticMesh(CylinderMesh.Object);
    }

    // 초기 스케일 0
    AOEMesh->SetWorldScale3D(FVector(0.f, 0.f, 0.1f));  // 높이는 낮게
}

void AAOEIndicator::BeginPlay()
{
    Super::BeginPlay();

    // Overlap 이벤트 바인딩
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(
        this,
        &AAOEIndicator::OnAOEBeginOverlap
    );

    // Dynamic Material 생성
    if (AOEMesh && AOEMesh->GetMaterial(0))
    {
        DynamicMaterial = AOEMesh->CreateAndSetMaterialInstanceDynamic(0);

        if (DynamicMaterial)
        {
            // 기본 색상 설정 (빨간색, 반투명)
            DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), FLinearColor::Red);
            DynamicMaterial->SetScalarParameterValue(FName("Opacity"), 0.5f);
        }
    }
}

void AAOEIndicator::StartAOE(
    AController* InInstigatorController,
    float InBaseDamage,
    float InMaxRadius,
    float InDuration,
    float InGrowthSpeed,
    float InDamageMultiplier)
{
    InstigatorController = InInstigatorController;
    BaseDamage = InBaseDamage;
    TargetRadius = InMaxRadius;
    TotalDuration = InDuration;
    GrowthRate = InGrowthSpeed;
    DamageMultiplier = InDamageMultiplier;

    ElapsedTime = 0.f;
    CurrentRadius = 0.f;
    DamagedActors.Empty();

    UE_LOG(LogTemp, Log, TEXT("[AOEIndicator] Started: Radius=%.1f, Duration=%.1f, Damage=%.1f"),
        TargetRadius, TotalDuration, BaseDamage * DamageMultiplier);
}

void AAOEIndicator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ElapsedTime += DeltaTime;

    // 반지름 증가
    CurrentRadius = FMath::Min(
        CurrentRadius + GrowthRate * DeltaTime,
        TargetRadius
    );

    // Collision 크기 업데이트
    CollisionSphere->SetSphereRadius(CurrentRadius);

    // Mesh 크기 업데이트 (XY만)
    UpdateMeshScale();
    // Material 투명도 업데이트
    UpdateMaterialOpacity();

    // 종료 조건 체크
    if (ElapsedTime >= TotalDuration)
    {
        UE_LOG(LogTemp, Log, TEXT("[AOEIndicator] Finished, destroying"));
        Destroy();
    }
}
void AAOEIndicator::OnAOEBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this)
        return;

    // 중복 데미지 방지
    if (DamagedActors.Contains(OtherActor))
    {
        return;  // 이미 맞춤
    }

    // Player만 공격
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (!Player)
    {
        return;  // Player가 아니면 무시
    }

    // 데미지 적용
    float FinalDamage = BaseDamage * DamageMultiplier;

    UGameplayStatics::ApplyDamage(
        OtherActor,
        FinalDamage,
        InstigatorController,
        this,
        UDamageType::StaticClass()
    );

    // 중복 방지 목록에 추가
    DamagedActors.Add(OtherActor);

    UE_LOG(LogTemp, Log, TEXT("[AOEIndicator] Damaged %s (%.1f damage)"),
        *OtherActor->GetName(), FinalDamage);
}

void AAOEIndicator::UpdateMeshScale()
{
    if (!AOEMesh) return;

    // Cylinder 기본 크기: 반지름 50, 높이 100
    // XY: 반지름에 맞춰 스케일
    // Z: 높이는 낮게 (바닥 표시용)
    float Scale = CurrentRadius / 50.f;
    AOEMesh->SetWorldScale3D(FVector(Scale, Scale, 0.1f));
}

void AAOEIndicator::UpdateMaterialOpacity()
{
    if (!DynamicMaterial) return;

    // 시간에 따라 투명도 감소 (1.0 → 0.0)
    float Alpha = FMath::Clamp(1.f - (ElapsedTime / TotalDuration), 0.f, 1.f);

    // 약간의 펄스 효과 (선택)
    float Pulse = FMath::Sin(ElapsedTime * 5.f) * 0.2f + 0.8f;
    float FinalOpacity = Alpha * Pulse * 0.5f;

    DynamicMaterial->SetScalarParameterValue(FName("Opacity"), FinalOpacity);
}
