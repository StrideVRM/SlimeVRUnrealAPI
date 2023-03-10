#include "SlimeVRButton.h"

USlimeVRButton* USlimeVRButton::CreateFromMessage(const FString& ButtonID, bool bIsPressed, float AnalogValue)
{
    USlimeVRButton* Button = NewObject<USlimeVRButton>();
    Button->SetButtonID(ButtonID);
    Button->bIsPressed = bIsPressed;
    Button->AnalogValue = AnalogValue;
    return Button;
}

void USlimeVRButton::SetButtonID(const FString& ID)
{
    if (ID.IsEmpty())
    {
        ButtonID = "unknown";
    }
    else
    {
        ButtonID = ID;
    }
}
