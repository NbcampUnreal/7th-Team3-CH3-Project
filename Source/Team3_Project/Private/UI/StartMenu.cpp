#include "UI/StartMenu.h"
#include "Kismet/GameplayStatics.h"

void UStartMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    if (Btn_Start)
    {
        // 버튼과 함수를 연결 
       // Btn_Start->OnClicked.AddDynamic(this, &UStartMenu::OnStartClicked);
    }
}

void UStartMenu::OnStartClicked()
{
    // 이동할 레벨 이름 
    //FName LevelName = TEXT("ThirdPersonMap");
    //UGameplayStatics::OpenLevel(GetWorld(), LevelName);

    RemoveFromParent();

    // 마우스 커서 숨기기 및 게임 입력 모드로 전환
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
}