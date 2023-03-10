#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SlimeVRButton.generated.h"

UCLASS(Blueprintable)
class SLIMEVR_API USlimeVRButton : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "SlimeVR Button")
    FString ButtonID;

    UPROPERTY(BlueprintReadOnly, Category = "SlimeVR Button")
    bool bIsPressed;

    UPROPERTY(BlueprintReadOnly, Category = "SlimeVR Button")
    float AnalogValue;

    static USlimeVRButton* CreateFromMessage(const FString& ButtonID, bool bIsPressed, float AnalogValue);

private:
    void SetButtonID(const FString& ID);
};
