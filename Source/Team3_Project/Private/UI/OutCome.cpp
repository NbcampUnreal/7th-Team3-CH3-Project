#include "UI/OutCome.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "EngineUtils.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

UOutCome::UOutCome(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    CurrentOpacity = 0.0f;
    FadeSpeed = 1.0f;      
    bStartFade = false;
}

void UOutCome::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &UOutCome::OnRestartClicked);
    if (Btn_Quit) Btn_Quit->OnClicked.AddDynamic(this, &UOutCome::OnQuitClicked);

    // 처음에는 투명하게 시작 
    SetRenderOpacity(0.0f);
}

void UOutCome::SetupOutcome(bool bIsVictory, int32 Score, int32 Kill)
{
    // 1. 결과 텍스트 및 색상 설정 
    if (bIsVictory)
    {
        Txt_ResultTitle->SetText(FText::FromString(TEXT("You WIN!")));
        Txt_ResultTitle->SetColorAndOpacity(FLinearColor::Green);
    }
    else
    {
        Txt_ResultTitle->SetText(FText::FromString(TEXT("Game Over")));
        Txt_ResultTitle->SetColorAndOpacity(FLinearColor::Red);
    }

    // 2. 점수 반영 
    Txt_FinalScore->SetText(FText::Format(FText::FromString(TEXT("Score : {0}")), FText::AsNumber(Score)));
    Txt_FinalKill->SetText(FText::Format(FText::FromString(TEXT("Kill : {0}")), FText::AsNumber(Kill)));

    // 3. 페이드 시작 플래그 
    bStartFade = true;
}

void UOutCome::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bStartFade && CurrentOpacity < 1.0f)
    {
        CurrentOpacity += InDeltaTime * FadeSpeed;
        SetRenderOpacity(CurrentOpacity); // 코드로 서서히 나타나게 함 
    }
}

void UOutCome::OnRestartClicked()
{

    UGameplayStatics::SetGamePaused(GetWorld(), false); // 게임 일시정지 해제
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
            {
        PC->bShowMouseCursor = false; // 마우스 커서 숨김
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode); // 게임 입력 모드로 전환
	}
	RemoveFromParent(); // UI 제거
    UGameplayStatics::OpenLevel(this, FName("Soul_Slum")); // 첫 화면 레벨명 입력 
}

void UOutCome::OnQuitClicked()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false); // 게임 일시정지 해제
    UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}