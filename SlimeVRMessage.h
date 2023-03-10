#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Json.h"
#include "SlimeVRMessage.generated.h"

UENUM(BlueprintType)
enum class ESlimeVRMessageType : uint8
{
    ObjectPosition UMETA(DisplayName = "Object Position"),
    ObjectRotation UMETA(DisplayName = "Object Rotation"),
    ButtonPress UMETA(DisplayName = "Button Press"),
    AnalogValue UMETA(DisplayName = "Analog Value"),
    PlayerJoined UMETA(DisplayName = "Player Joined"),
    PlayerLeft UMETA(DisplayName = "Player Left"),
    Handshake UMETA(DisplayName = "Handshake"),
};

UCLASS(Blueprintable)
class SLIMEVR_API USlimeVRMessage : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "SlimeVR Message")
    ESlimeVRMessageType MessageType;

    UPROPERTY(BlueprintReadOnly, Category = "SlimeVR Message")
    FString MessageData;

    static USlimeVRMessage* CreateFromJson(const FString& JsonString);

    FString MessageTypeString() const;

private:
    static TMap<ESlimeVRMessageType, FString> MessageTypeStringMap;
};
