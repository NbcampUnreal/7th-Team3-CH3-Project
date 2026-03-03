#include "UI/StartMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Widget.h"

UStartMenu::UStartMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    CurrentAlpha = 1.0f;
    FadeDir = 1.0f;   
    bIsTransitioning = false;
}

void UStartMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    TArray<UUserWidget*> FoundWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), false);

    for (UUserWidget* Widget : FoundWidgets)
    {
        // 나(StartMenu)를 제외한 'MainHUD'라는 이름이 포함된 위젯을 찾아 숨김
        if (Widget && Widget != this && Widget->GetName().Contains(TEXT("MainHUD")))
        {
            Widget->SetVisibility(ESlateVisibility::Collapsed); // 완전히 숨김 
        }
    }

    if (Btn_Start)
    {
        // 버튼과 함수를 연결 
       Btn_Start->OnClicked.AddDynamic(this, &UStartMenu::OnStartClicked);
    }
}

void UStartMenu::OnStartClicked()
{
    bIsTransitioning = true;
    CurrentAlpha = 0.0f;

    // 버튼 여러 번 누르지 못하게 비활성화 
    if (Btn_Start)
    {
        Btn_Start->SetIsEnabled(false);
    }
}

void UStartMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsTransitioning)
    {
        // CurrentAlpha를 0에서 1로 증가시킴 (약 0.7초 소요)
        CurrentAlpha += InDeltaTime * FadeDir * 1.5f;

        // 위젯 전체의 투명도 조절 (1.0이 밝음, 0.0이 어두움)
        SetRenderOpacity(1.0f - FMath::Clamp(CurrentAlpha, 0.0f, 1.0f));

        // 완전히 어두워졌다면 레벨 이동 
        if (CurrentAlpha >= 1.0f)
        {
            bIsTransitioning = false;

            UGameplayStatics::OpenLevel(GetWorld(), TEXT("ThirdPersonMap"));

            // 입력 모드 복구 (새 맵으로 넘어가기 전 안전하게 설정)
            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                FInputModeGameOnly InputMode;
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = false;
            }
        }
    }
}

/*void UStartMenu::MoveToGameLevel()
{
    // 레벨 이동
    FName LevelName = TEXT("ThirdPersonMap");
    UGameplayStatics::OpenLevel(GetWorld(), LevelName);

    // 마우스 커서 숨기기 및 게임 입력 모드
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
}*/