#include "UI/CompassWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

void UCompassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();
	APawn* PlayerPawn = GetOwningPlayerPawn();

	if (PC && PC->PlayerCameraManager && PlayerPawn)
	{
		float CamYaw = PC->PlayerCameraManager->GetCameraRotation().Yaw;
		float PawnYaw = PlayerPawn->GetActorRotation().Yaw;

		if (OuterCompass)
		{
			//OuterCompass->SetRenderTransformAngle(-CamYaw);
			OuterCompass->SetRenderTransformAngle(0.0f);
		}

		if (InnerNeedle)
		{
			//float FinalAngle = -PawnYaw;
			//InnerNeedle->SetRenderTransformAngle(FinalAngle);
			//UE_LOG(LogTemp, Warning, TEXT("Pawn Yaw: %f | Final Angle: %f"), PawnYaw, FinalAngle);

			InnerNeedle->SetRenderTransformAngle(PawnYaw);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InnerNeedle is NOT bound!"));
		}
	}
}
