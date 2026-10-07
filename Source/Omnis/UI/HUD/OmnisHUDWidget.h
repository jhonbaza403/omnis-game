#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OmnisHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class OMNIS_API UOmnisHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UOmnisHUDWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|HUD")
    void RefreshHUD();

protected:
    UPROPERTY()
    TObjectPtr<UTextBlock> TitleText;

    UPROPERTY()
    TObjectPtr<UTextBlock> WorldText;

    UPROPERTY()
    TObjectPtr<UTextBlock> HealthText;

    UPROPERTY()
    TObjectPtr<UTextBlock> ControlsText;

    UPROPERTY()
    TObjectPtr<UTextBlock> NexusText;

    UPROPERTY()
    TObjectPtr<UTextBlock> ProgressionText;

    UPROPERTY()
    TObjectPtr<UTextBlock> SocialText;

    UPROPERTY()
    TObjectPtr<UProgressBar> HealthBar;

private:
    void BuildLayout();

    float RefreshAccumulator = 0.0f;
};
