#include "SlimeVRMessage.h"

TMap<ESlimeVRMessageType, FString> USlimeVRMessage::MessageTypeStringMap = {
    { ESlimeVRMessageType::ObjectPosition, "object_position" },
    { ESlimeVRMessageType::ObjectRotation, "object_rotation" },
    { ESlimeVRMessageType::ButtonPress, "button_press" },
    { ESlimeVRMessageType::AnalogValue, "analog_value" },
    { ESlimeVRMessageType::PlayerJoined, "player_joined" },
    { ESlimeVRMessageType::PlayerLeft, "player_left" },
    { ESlimeVRMessageType::Handshake, "handshake" }
};

USlimeVRMessage* USlimeVRMessage::CreateFromJson(const FString& JsonString)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
    {
        FString MessageTypeString;
        if (JsonObject->TryGetStringField("type", MessageTypeString))
        {
            ESlimeVRMessageType MessageType = static_cast<ESlimeVRMessageType>(0);
            FString* MessageTypeStringPtr = MessageTypeStringMap.FindKey(MessageTypeString);
            if (MessageTypeStringPtr != nullptr)
            {
                MessageType = *MessageTypeStringPtr;
            }

            FString MessageData;
            if (JsonObject->TryGetStringField("data", MessageData))
            {
                USlimeVRMessage* Message = NewObject<USlimeVRMessage>();
                Message->MessageType = MessageType;
                Message->MessageData = MessageData;
                return Message;
            }
        }
    }

    return nullptr;
}

FString USlimeVRMessage::MessageTypeString() const
{
    FString* MessageTypeStringPtr = MessageTypeStringMap.Find(MessageType);
    if (MessageTypeStringPtr != nullptr)
    {
        return *MessageTypeStringPtr;
    }

    return FString();
}
