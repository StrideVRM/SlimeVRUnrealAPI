#include "SlimeVRCommunication.h"

USlimeVRCommunication::USlimeVRCommunication()
    : Socket(nullptr), RemoteAddress(nullptr)
{
}

USlimeVRCommunication::~USlimeVRCommunication()
{
    Disconnect();
}

bool USlimeVRCommunication::Connect(const FString& IPAddressString, int32 Port)
{
    if (Socket != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("SlimeVR socket is already connected"));
        return false;
    }

    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("SlimeVR"), false);
    if (Socket == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create SlimeVR socket"));
        return false;
    }

    TSharedPtr<FInternetAddr> Address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValidIP = false;
    Address->SetIp(*IPAddressString, bIsValidIP);
    Address->SetPort(Port);

    if (!bIsValidIP)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid SlimeVR IP address: %s"), *IPAddressString);
        return false;
    }

    RemoteAddress = Address;
    if (Socket->Connect(*RemoteAddress))
    {
        UE_LOG(LogTemp, Display, TEXT("SlimeVR socket connected to %s:%d"), *IPAddressString, Port);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to connect SlimeVR socket to %s:%d"), *IPAddressString, Port);
        Socket = nullptr;
        return false;
    }
}

void USlimeVRCommunication::Disconnect()
{
    if (Socket != nullptr)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
        RemoteAddress = nullptr;
        ReceivedData.Empty();
    }
}

bool USlimeVRCommunication::IsConnected() const
{
    return Socket != nullptr && RemoteAddress != nullptr;
}

void USlimeVRCommunication::RegisterMessageCallback(const FString& MessageType, FSlimeVRMessageCallback Callback)
{
    MessageCallbacks[MessageType] = Callback;
}

void USlimeVRCommunication::UnregisterMessageCallback(const FString& MessageType)
{
    MessageCallbacks.erase(MessageType);
}

void USlimeVRCommunication::SendMessage(const USlimeVRMessage* Message)
{
    if (Socket != nullptr)
    {
        FString JsonString;
        if (FJsonObjectConverter::UStructToJsonObjectString(Message->StaticStruct(), Message, JsonString, 0, 0))
        {
            FString FullJsonString = FString::Printf(TEXT("%s\n"), *JsonString);
            TArray<uint8> Data;
            FTCHARToUTF8 Converter(*FullJsonString);
            const char* Utf8String = Converter.Get();
            int32 NumBytes = FCStringAnsi::Strlen(Utf8String);
            Data.Append((const uint8*)Utf8String, NumBytes);
            Socket->Send(Data.GetData(), NumBytes);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to serialize SlimeVR message"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SlimeVR socket is not connected"));
    }
}

void USlimeVRCommunication::Update()
{
    if (Socket != nullptr)
    {
        uint32 PendingDataSize = 0;
        if (Socket->HasPendingData(PendingDataSize))
        {
            ReceivedData.SetNumUninitialized(PendingDataSize);
        }
        if (Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), /*bPeek=*/false) > 0)
        {
            ParseReceivedData(ReceivedData);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to receive SlimeVR data"));
            Disconnect();
        }
    }
}

void USlimeVRCommunication::ParseReceivedData(const TArray<uint8>& Data)
{
    FString JsonString;
    for (uint8 Byte : Data)
    {
        if (Byte == '\n')
        {
            USlimeVRMessage* Message = USlimeVRMessage::CreateFromJsonString(JsonString);
            if (Message != nullptr)
            {
                HandleMessage(Message);
            }
            JsonString.Empty();
        }
        else
        {
            JsonString.AppendChar(Byte);
        }
    }
}

void USlimeVRCommunication::HandleMessage(const USlimeVRMessage* Message)
{
    const FString& MessageType = Message->MessageType;
    auto CallbackIter = MessageCallbacks.find(MessageType);
    if (CallbackIter != MessageCallbacks.end())
    {
        FSlimeVRMessageCallback Callback = CallbackIter->second;
        Callback.Execute(Message);
    }
}
