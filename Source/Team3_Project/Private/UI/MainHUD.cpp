#include "UI/MainHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "UI/QuestItemWidget.h"
#include "UI/ResultWidget.h"
#include "Item/InventoryComponent.h"
#include "Core/ItemDataSubsystem.h"
#include "Shared/ItemTypes.h"
#include "Item/WeaponItem.h"
#include "Player/PlayerCharacter.h"
#include "Core/MainGameState.h"

UMainHUD::UMainHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    CurrHealth = TargetHealth = 1.0f;
    CurrStamina = TargetStamina = 1.0f;
    CurrWhiteKarma = TargetWhiteKarma = 0.5f;
    CurrBlackKarma = TargetBlackKarma = 0.5f;
    PendingBonus = 100;
    bPendingSuccess = true;
}

void UMainHUD::NativeConstruct()
{
    Super::NativeConstruct();

    SlotArray = { Slot_0, Slot_1, Slot_2, Slot_3, Slot_4, Slot_5, Slot_6, Slot_7 };
    SlotNumArray = { Txt_SlotNum_1, Txt_SlotNum_2, Txt_SlotNum_3, Txt_SlotNum_4,
                     Txt_SlotNum_5, Txt_SlotNum_6, Txt_SlotNum_7, Txt_SlotNum_8 };
    /* 델리게이트 바인딩
    OnWhiteKarmaChanged.AddDynamic(this, &UMainHUD::UpdateWhiteKarma);
    OnBlackKarmaChanged.AddDynamic(this, &UMainHUD::UpdateBlackKarma);
    OnScoreChanged.AddDynamic(this, &UMainHUD::UpdateScore);
    OnKillsChanged.AddDynamic(this, &UMainHUD::UpdateKills);
    OnHealthChanged.AddDynamic(this, &UMainHUD::UpdateHealth);
    OnStaminaChanged.AddDynamic(this, &UMainHUD::UpdateStamina);
    OnQuestAdded.AddDynamic(this, &UMainHUD::AddNewQuest);
    OnQuestFinished.AddDynamic(this, &UMainHUD::FinishQuest);
    OnWaveStarted.AddDynamic(this, &UMainHUD::StartWaveUI);
    OnWaveEnded.AddDynamic(this, &UMainHUD::ReceiveTeamData);*/
   
    /*AMainGameState* GameState = GetWorld()->GetGameState<AMainGameState>();
    if (GameState)
    {
        GameState->OnWaveStart.AddDynamic(this, &UMainHUD::StartWaveUI);
        GameState->OnWaveEnd.AddDynamic(this, &UMainHUD::EndWaveUI);
    }*/

    APawn* PlayerPawn = GetOwningPlayerPawn();
    if (PlayerPawn)
    {
        UInventoryComponent* InvComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
        if (InvComp)
        {
            InvComp->OnQuickSlotUpdated.AddDynamic(this, &UMainHUD::OnQuickSlotRefreshAll);
            InvComp->OnQuickSlotItemChanged.AddDynamic(this, &UMainHUD::OnQuickSlotItemChanged);

            InvComp->OnWeaponChanged.AddDynamic(this, &UMainHUD::OnWeaponEquipChanged);

            UpdateQuickSlotUI();
        }
    }
   
    /*APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(PlayerPawn);
    if (PlayerChar)
    {
        PlayerChar->OnHealthChanged.AddDynamic(this, &UMainHUD::UpdateHealth);
        PlayerChar->OnStaminaChanged.AddDynamic(this, &UMainHUD::UpdateStamina);
        //PlayerChar->OnWhiteKarmaChanged.AddDynamic(this, &UMainHUD::UpdateWhiteKarma);
        //PlayerChar->OnBlackKarmaChanged.AddDynamic(this, &UMainHUD::UpdateBlackKarma);
    }*/

    if (Timer) Timer->SetVisibility(ESlateVisibility::Collapsed);
    RefreshQuestIcon();

    if (Txt_CurrentAmmo) Txt_CurrentAmmo->SetVisibility(ESlateVisibility::Collapsed);
    if (Txt_MaxAmmo) Txt_MaxAmmo->SetVisibility(ESlateVisibility::Collapsed);
    if (Txt_Divider) Txt_Divider->SetVisibility(ESlateVisibility::Collapsed);

    if (Img_HitMarker) Img_HitMarker->SetRenderOpacity(0.0f);
}

void UMainHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // WhiteKarma
    CurrWhiteKarma = FMath::FInterpTo(CurrWhiteKarma, TargetWhiteKarma, InDeltaTime, InterpSpeed);
    if (PB_WhiteKarma) PB_WhiteKarma->SetPercent(CurrWhiteKarma);

    // BlackKarma
    CurrBlackKarma = FMath::FInterpTo(CurrBlackKarma, TargetBlackKarma, InDeltaTime, InterpSpeed);
    if (PB_BlackKarma) PB_BlackKarma->SetPercent(CurrBlackKarma);

    // Health
    CurrHealth = FMath::FInterpTo(CurrHealth, TargetHealth, InDeltaTime, InterpSpeed);
    if (PB_Health)
    {
        PB_Health->SetPercent(CurrHealth);

        FLinearColor TargetColor;
        if (CurrHealth > 0.5f)
        {
            // 0.5(주황) ~ 1.0(초록) 사이를 부드럽게 연결 
            float Alpha = (CurrHealth - 0.5f) * 2.0f;
            TargetColor = FLinearColor::LerpUsingHSV(FLinearColor(1.0f, 0.5f, 0.0f), FLinearColor::Green, Alpha);
        }
        else
        {
            // 0.0(빨강) ~ 0.5(주황) 사이를 부드럽게 연결 
            float Alpha = CurrHealth * 2.0f;
            TargetColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor(1.0f, 0.5f, 0.0f), Alpha);
        }
        PB_Health->SetFillColorAndOpacity(TargetColor);
    }

    // Stamina
    CurrStamina = FMath::FInterpTo(CurrStamina, TargetStamina, InDeltaTime, InterpSpeed);
    if (PB_Stamina)
    {
        PB_Stamina->SetPercent(CurrStamina);

        FLinearColor TargetColor;
        if (CurrStamina > 0.5f)
        {
            float Alpha = (CurrStamina - 0.5f) * 2.0f;
            TargetColor = FLinearColor::LerpUsingHSV(FLinearColor(1.0f, 0.5f, 0.0f), FLinearColor::Green, Alpha);
        }
        else
        {
            float Alpha = CurrStamina * 2.0f;
            TargetColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor(1.0f, 0.5f, 0.0f), Alpha);
        }
        PB_Stamina->SetFillColorAndOpacity(TargetColor);
    }

    // Timer (/ EndWaveUI)
    if (bIsTimerActive)
    {
        CurrentRemainingTime -= InDeltaTime;
        
        if (Timer)
        {
            int32 TotalSeconds = FMath::Max(0, FMath::FloorToInt(CurrentRemainingTime));
            int32 Min = TotalSeconds / 60;
            int32 Sec = TotalSeconds % 60;
            FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Min, Sec);
            Timer->SetText(FText::FromString(TimeStr));

            if (CurrentRemainingTime <= 10.0f)
            {
                Timer->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
            }
            else
            {
                Timer->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            }
        }

        if (CurrentRemainingTime <= 0.0f)
        {
            bIsTimerActive = false;
            EndWaveUI(bPendingSuccess, PendingBonus);
        }
    }
}

void UMainHUD::AddNewQuest(int32 ID, FString Title, FString Desc, int32 MaxCount)
{
    if (QuestList && QuestItemClass)
    {
        UQuestItemWidget* NewItem = CreateWidget<UQuestItemWidget>(this, QuestItemClass);
        if (NewItem)
        {
            NewItem->UpdateQuestProgress(Title, Desc, 0, MaxCount);
            QuestList->AddChildToVerticalBox(NewItem);
            QuestMap.Add(ID, NewItem);
            RefreshQuestIcon();
        }
    }
}

void UMainHUD::FinishQuest(int32 ID)
{
    if (QuestMap.Contains(ID))
    {
        QuestMap[ID]->CompleteQuest();
        QuestMap.Remove(ID); // 맵에서 즉시 제거하여 중복 방지 

        FTimerHandle IconHandle;
        GetWorld()->GetTimerManager().SetTimer(IconHandle, this, &UMainHUD::RefreshQuestIcon, 3.1f, false);
    }
}

void UMainHUD::UpdateScore(int32 Score)
{
    CurrentScore = Score;
    if (TotalScore)
    {
        TotalScore->SetText(FText::Format(FText::FromString("Score : {0}"), Score));
    }
}

void UMainHUD::UpdateKills(int32 Kills) 
{
    CurrentKills = Kills; 
    if (TotalKills)
    {
        TotalKills->SetText(FText::Format(FText::FromString("Kill : {0}"), Kills));
    }
}

void UMainHUD::UpdateHealth(float Health) 
{ 
    TargetHealth = FMath::Clamp(Health / 100.0f, 0.0f, 1.0f);
}

void UMainHUD::UpdateStamina(float Stamina) 
{
    TargetStamina = FMath::Clamp(Stamina / 100.0f, 0.0f, 1.0f);
}

void UMainHUD::UpdateWhiteKarma(float White) 
{ 
    TargetWhiteKarma = FMath::Clamp(White / 100.0f, 0.0f, 1.0f);
}

void UMainHUD::UpdateBlackKarma(float Black) 
{ 
    TargetBlackKarma = FMath::Clamp(Black / 100.0f, 0.0f, 1.0f);
}

void UMainHUD::StartWaveUI(FString Message, float WaveTime)
{
    TempWaveScore = CurrentScore;
    TempWaveKills = CurrentKills;
    CurrentRemainingTime = WaveTime;
    bIsTimerActive = true;

    if (Timer)
    {
        Timer->SetVisibility(ESlateVisibility::Visible);
    }
    if (Txt_WaveMessage)
    {
        Txt_WaveMessage->SetText(FText::FromString(Message));
        Txt_WaveMessage->SetVisibility(ESlateVisibility::Visible);

        FTimerHandle MsgTimerHandle;
 
        GetWorld()->GetTimerManager().SetTimer(
            MsgTimerHandle,
            this,
            &UMainHUD::HideWaveMessage,
            3.0f,
            false
        );
    }
}

void UMainHUD::EndWaveUI(bool bSuccess, int32 Bonus)
{
    bIsTimerActive = false;
    if (Timer)
    {
        Timer->SetVisibility(ESlateVisibility::Collapsed);
    }

    int32 FinalWaveScore = CurrentScore - TempWaveScore; 

    if (bSuccess)
    {
        CurrentScore += Bonus;
        UpdateScore(CurrentScore);
    }

    int32 BonusToSend = bSuccess ? Bonus : 0;

    if (ResultWidgetClass)
    {
        UResultWidget* ResultUI = CreateWidget<UResultWidget>(GetWorld(), ResultWidgetClass);
        if (ResultUI)
        {
            ResultUI->AddToViewport();
            ResultUI->SetupSuccess(CurrentKills - TempWaveKills, FinalWaveScore, Bonus, bSuccess);
        }
    }
}

void UMainHUD::RefreshQuestIcon() 
{ 
    if (img_Quest)
    {
        img_Quest->SetVisibility(QuestMap.Num() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UMainHUD::HideWaveMessage()
{
    if (Txt_WaveMessage)
    {
        Txt_WaveMessage->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UMainHUD::ReceiveTeamData(bool bSuccess, int32 Bonus)
{
    bPendingSuccess = bSuccess;
    PendingBonus = Bonus;
}

void UMainHUD::UpdateQuickSlotImage(int32 SlotIndex, UTexture2D* IconTexture)
{
    if (SlotArray.IsValidIndex(SlotIndex))
    {
        if (IconTexture)
        {
            // 아이템 O 
            SlotArray[SlotIndex]->SetBrushFromTexture(IconTexture);
            SlotArray[SlotIndex]->SetOpacity(1.0f);
        }
        else
        {
            // 아이템 X 
            SlotArray[SlotIndex]->SetOpacity(0.2f);
        }
    }
}

void UMainHUD::OnQuickSlotItemChanged(int32 SlotIndex, FName ItemID)
{
   /* UItemDataSubsystem* ItemSubsystem = GetGameInstance()->GetSubsystem<UItemDataSubsystem>();
    if (ItemSubsystem)
    {
        FItemData ItemData = ItemSubsystem->GetItemDataByID(ItemID);

        // ID가 비어있지 않은지 확인 -> 유효한 데이터인지 체크
        if (!ItemData.ItemID.IsNone())
        {
            // TSoftObjectPtr인 Icon을 동기 로드 
            UTexture2D* LoadedIcon = ItemData.Icon.LoadSynchronous();
            UpdateQuickSlotImage(SlotIndex, LoadedIcon);
        }
        else
        {
            // 데이터가 없으면 슬롯 비우기
            UpdateQuickSlotImage(SlotIndex, nullptr);
        }
    }*/

    UItemDataSubsystem* ItemSubsystem = GetGameInstance()->GetSubsystem<UItemDataSubsystem>();
    if (!ItemSubsystem) return;

    if (!ItemID.IsNone())
    {
        FItemData ItemData = ItemSubsystem->GetItemDataByID(ItemID);
        if (!ItemData.ItemID.IsNone())
        {
            UTexture2D* LoadedIcon = ItemData.Icon.LoadSynchronous();
            UpdateQuickSlotImage(SlotIndex, LoadedIcon);
            return;
        }
    }

    UpdateQuickSlotImage(SlotIndex, nullptr);
}

void UMainHUD::UpdateQuickSlotUI()
{
    APawn* PlayerPawn = GetOwningPlayerPawn();
    if (PlayerPawn)
    {
        UInventoryComponent* InvComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
        if (InvComp)
        {
            for (int32 i = 0; i < InvComp->QuickSlots.Num(); ++i)
            {
                if (SlotArray.IsValidIndex(i))
                {
                    FName ItemID = InvComp->QuickSlots[i];
                    OnQuickSlotItemChanged(i, ItemID);
                }
            }
        }
    }
}

void UMainHUD::OnEquipmentChanged()
{
    // 장비 변경 시 필요한 로직 (나중에 구현)
    UpdateQuickSlotUI();
}

void UMainHUD::OnQuickSlotRefreshAll()
{
    UpdateQuickSlotUI();
}

void UMainHUD::HighlightQuickSlot(int32 SlotIndex)
{
    for (int32 i = 0; i < SlotNumArray.Num(); ++i)
    {
        if (SlotNumArray[i])
        {
            // 선택된 인덱스(i == SlotIndex)면 빨강, 아니면 하양
            FLinearColor TargetColor = (i == SlotIndex) ? FLinearColor::Red : FLinearColor::White;
            SlotNumArray[i]->SetColorAndOpacity(FSlateColor(TargetColor));

        }
    }
}

void UMainHUD::OnWeaponEquipChanged(bool bIsEquipping, FName ItemID)
{
    if (!ItemDataSubsystem)
    {
        ItemDataSubsystem = GetGameInstance()->GetSubsystem<UItemDataSubsystem>();
    }

    if (bIsEquipping && !ItemID.IsNone() && ItemDataSubsystem)
    {
        FItemData Data = ItemDataSubsystem->GetItemDataByID(ItemID);

        if (!Data.ItemID.IsNone())
        {
            UTexture2D* IconTexture = Data.WeaponImage.LoadSynchronous();

            if (IconTexture)
            {
                Img_GunInformation->SetBrushFromTexture(IconTexture);  
            }

            if (bIsEquipping)
            {
                Img_GunInformation->SetOpacity(1.0f);
            }
            else
            {
                Img_GunInformation->SetOpacity(0.3f);
            }
        }
    }
    /*else
    {
        //Img_GunInformation->SetVisibility(ESlateVisibility::Hidden);

        if (Txt_CurrentAmmo && Txt_Divider && Txt_MaxAmmo)
        {
            Txt_CurrentAmmo->SetVisibility(ESlateVisibility::Hidden);
            Txt_Divider->SetVisibility(ESlateVisibility::Hidden);
            Txt_MaxAmmo->SetVisibility(ESlateVisibility::Hidden);
        }
    }*/

    APawn* PlayerPawn = GetOwningPlayerPawn();
    if (PlayerPawn)
    {
        UInventoryComponent* InvComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
        if (InvComp)
        {
            AWeaponItem* EquippedWeapon = InvComp->GetEquippedWeapon();
            if (EquippedWeapon)
            {
                EquippedWeapon->OnAmmoChanged.RemoveDynamic(this, &UMainHUD::OnAmmoChanged);
                if (bIsEquipping)
                {
                    EquippedWeapon->OnAmmoChanged.AddDynamic(this, &UMainHUD::OnAmmoChanged);
                    OnAmmoChanged(0,0);
                }
                else
                {
                    Txt_CurrentAmmo->SetVisibility(ESlateVisibility::Hidden);
                    Txt_Divider->SetVisibility(ESlateVisibility::Hidden);
                    Txt_MaxAmmo->SetVisibility(ESlateVisibility::Hidden);
                }
            }
        }
    }
}

void UMainHUD::OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (Txt_CurrentAmmo && Txt_MaxAmmo)
    {
        if (Txt_CurrentAmmo->GetVisibility() == ESlateVisibility::Collapsed)
        {
            Txt_CurrentAmmo->SetVisibility(ESlateVisibility::Visible);
            Txt_MaxAmmo->SetVisibility(ESlateVisibility::Visible);
            Txt_Divider->SetVisibility(ESlateVisibility::Visible);
        }

        Txt_CurrentAmmo->SetText(FText::AsNumber(CurrentAmmo));
        Txt_MaxAmmo->SetText(FText::AsNumber(MaxAmmo));

        FString AmmoStr = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
        

        if (CurrentAmmo <= 10)
        {
            Txt_CurrentAmmo->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
        }
        else
        {
            Txt_CurrentAmmo->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        }
    }
}

void UMainHUD::UpdateQuestCount(int32 ID, FString Title, FString Desc, int32 Current, int32 Max)
{
    if (QuestMap.Contains(ID))
    {
        QuestMap[ID]->UpdateQuestProgress(Title, Desc, Current, Max);

        if (Current >= Max)
        {
            FinishQuest(ID);
        }
    }
}

void UMainHUD::UpdateHitMarker(bool bIsHit, bool bIsDead)
{
    if (!bIsHit || !Img_HitMarker) return;

    FLinearColor MarkerColor = bIsDead ? FLinearColor::Red : FLinearColor::White;
    Img_HitMarker->SetColorAndOpacity(MarkerColor);

    Img_HitMarker->SetRenderOpacity(1.0f);

    GetWorld()->GetTimerManager().ClearTimer(HitMarkerTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(HitMarkerTimerHandle, this, &UMainHUD::HideHitMarker, 0.5f, false);
}

void UMainHUD::HideHitMarker()
{
    if (Img_HitMarker)
    {
        Img_HitMarker->SetRenderOpacity(0.0f);
    }
}