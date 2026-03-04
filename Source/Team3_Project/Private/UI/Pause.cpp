#include "UI/Pause.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UPause::NativeConstruct()
{
    Super::NativeConstruct();

    if (Btn_Back) Btn_Back->OnClicked.AddDynamic(this, &UPause::OnBackClicked);
    if (Btn_MainMenu) Btn_MainMenu->OnClicked.AddDynamic(this, &UPause::OnMainMenuClicked);
}

void UPause::OnBackClicked()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetPause(false); // 게임 일시정지 해제 
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
    RemoveFromParent(); // 메뉴 닫기 
}

void UPause::OnMainMenuClicked()
{
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Soul_Slum"));
}