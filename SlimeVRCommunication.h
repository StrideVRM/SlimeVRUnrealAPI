#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SlimeVRMessage.h"
#include "SlimeVRButton.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FSlimeVRMessageCallback, const USlimeVRMessage*, Message);

UCLASS(Blueprintable, Category = "SlimeVR Communication")
class SLIMEVR_API USlimeVRCommunication : public UObject
{
    GENERATED_BODY()

public:
    USlimeVRCommunication();
    virtual ~USlimeVRCommunication();

    UFUNCTION(BlueprintCallable, Category = "SlimeVR Communication")
    bool Connect(const FString& IPAddressString, int32 Port);

    UFUNCTION(BlueprintCallable, Category = "SlimeVR Communication")
    void Disconnect();

    UFUNCTION(BlueprintPure, Category = "SlimeVR Communication")
    bool IsConnected() const;

    void RegisterMessageCallback(const FString& MessageType, FSlimeVRMessageCallback Callback);
    void UnregisterMessageCallback(const FString& MessageType);

    UFUNCTION(BlueprintCallable, Category = "SlimeVR Communication")
    void SendMessage(const USlimeVRMessage* Message);

    void Update();

private:
    void ParseReceivedData(const TArray<uint8>& Data);
    void HandleMessage(const USlimeVRMessage* Message);

    TSharedPtr<FInternetAddr> RemoteAddress;
    FSocket* Socket;

    TMap<FString, FSlimeVRMessageCallback> MessageCallbacks;
    TArray<uint8> ReceivedData;
};

