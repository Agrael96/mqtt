#include "mqtt_packets.h"

#include <string.h>

static uint8_t mqtt_calculateRemainingLength(uint32_t messageLength, uint8_t remainingLength [4]){
    uint8_t it = 0;
    do{
        remainingLength[it] = messageLength % 128;
        messageLength /= 128;
        if(messageLength > 0){
            remainingLength[it] |=128;
        }
        it++;
    }
    while(messageLength > 0);
    return it;
}

static uint8_t mqtt_calculateRealLength(uint8_t remainingLength [4], uint32_t* realLength){
    uint8_t it = 0;
    uint32_t multiplyer = 1;
    do{
        if(it > 3){
            *realLength = 0;
            return 0;
        }
        *realLength += (remainingLength[it] & 127) * multiplyer;
        multiplyer *= 128;
        it++;
    }while(realLength[it-1] & 128);
    return it;
}

uint32_t mqtt_MqttConnectSerialize(uint8_t* const buffer, MqttConnect* const message){

    uint8_t* it = buffer;

    uint32_t messageSize = 10 + (2 + message->clientIdLength);
    if(message->user && message->pass){
        messageSize +=  2 + message->userLength + 2 + message->passLength;
    }
    uint8_t remLength [4];
    uint8_t byteNum = mqtt_calculateRemainingLength(messageSize, remLength);

    message->remainingLength = remLength;

    it = (uint8_t*)memcpy(it, &message->messageType, sizeof(message->messageType));
    it = (uint8_t*)memcpy(it+sizeof(message->messageType), message->remainingLength, byteNum);
    it = (uint8_t*)memcpy(it+byteNum, &message->length, 12);
    it = (uint8_t*)memcpy(it+12, message->clientId, message->clientIdLength);
    it = (uint8_t*)memcpy(it+message->clientIdLength, &message->userLength, 2);
    it = (uint8_t*)memcpy(it+2, message->user, message->userLength);
    it = (uint8_t*)memcpy(it+message->userLength, &message->passLength, 2);
    it = (uint8_t*)memcpy(it+2, message->pass, message->passLength);

    return messageSize + byteNum + 1;
}

uint32_t mqtt_MqttDisconnectSerialize(uint8_t* const buffer, const MqttDisconnect* const message){
    memcpy(buffer, message, sizeof(MqttDisconnect));
    return sizeof(MqttDisconnect);
}

int mqtt_MqttConnackDeserialize(uint8_t* message, MqttConnack* const output){
    if(message[0] == CONNACK){
        memcpy(output, message, sizeof(MqttConnack));
        return 1;
    }
    return 0;
}

uint32_t mqtt_MqttPublishSerialize(uint8_t* buffer, MqttPublish* const message){
    uint8_t* it = buffer;
    uint8_t remainingLength [4];
    uint32_t messageLength = sizeof(message->topicLength) + message->topicLength +
                                            sizeof(message->pid) + strlen(message->payload);
    uint8_t byteNum = mqtt_calculateRemainingLength(messageLength, remainingLength);
    message->remainingLength = remainingLength;
    message->messageType = PUBLISH;

    it = (uint8_t*) memcpy(it, message, 1);
    it = (uint8_t*) memcpy(it + 1, message->remainingLength, byteNum);
    it = (uint8_t*) memcpy(it + byteNum, &message->topicLength, 2);
    it = (uint8_t*) memcpy(it+2, message->topic, message->topicLength);
    it = (uint8_t*) memcpy(it+message->topicLength, &message->pid, 2);
    it = (uint8_t*) memcpy(it+2, message->payload, strlen(message->payload));
    return messageLength + byteNum + 1;
}

int mqtt_MqttPublishDeserialize(uint8_t* message, MqttPublish* const output){
    if(message[0] == (PUBLISH << 4)){
        memcpy(output, message, 1);

        uint32_t messageLength = 0;
        uint8_t remainingLengthByteNum = 0;
        if((remainingLengthByteNum = mqtt_calculateRealLength(message+1, &messageLength))){
            output->remainingLength = message + 1;
            output->topicLength = message[1+remainingLengthByteNum] << 8 | message[2+remainingLengthByteNum];
            output->topic = (char*)(message + 1 + sizeof(output->topicLength) + remainingLengthByteNum);
            uint8_t* it = message + 1 + remainingLengthByteNum+sizeof(output->topicLength) + output->topicLength;
            memcpy(&output->pid, it, sizeof(output->pid));
            output->payload = (char*) (it+sizeof(output->pid));
        }
        return 1;
    }
    return 0;
}

uint32_t mqtt_MqttPubackSerialize(uint8_t* buffer, const MqttPuback* const message){
    memcpy(buffer, message, sizeof(MqttPuback));
    return sizeof(MqttPuback);
}

int mqtt_MqttPubackDeserialize(uint8_t* message, MqttPuback* const output){
    if(message[0] == PUBACK){
        memcpy(output, message, sizeof(MqttPuback));
    }
    return 0;
}

uint32_t mqtt_MqttPubrelSerialize(uint8_t* buffer, const MqttPubrel* const message){
    memcpy(buffer, message, sizeof(MqttPubrel));
    return sizeof(MqttPubrel);
}

int mqtt_MqttPubrelDeserialize(uint8_t* message, MqttPubrel* const output){
    if(message[0] == PUBREL){
        memcpy(output, message, sizeof(MqttPubrel));
        return 1;
    }
    return 0;
}

uint32_t mqtt_MqttPubcompSerialize(uint8_t* buffer, const MqttPubcomp* const message){
    memcpy(buffer, message, sizeof(MqttPubcomp));
    return sizeof(MqttPubcomp);
}

int mqtt_MqttPubcompDeserialize(uint8_t* message, MqttPubcomp* const output){
    if(message[0] == PUBCOMP){
        memcpy(output, message, sizeof(MqttPubcomp));
        return 1;
    }
    return 0;
}

uint32_t mqtt_MqttSubscribeSerialize(uint8_t* buffer, MqttSubscribe* const message){
    uint8_t* it = buffer;
    uint32_t messageLength = sizeof(message->pid) + sizeof(message->topicLength) +
                                            message->topicLength + sizeof(message->qos);
    uint8_t remainingLength [4];
    uint8_t byteNum = mqtt_calculateRemainingLength(messageLength, remainingLength);
    message->remainingLength = remainingLength;

    it = (uint8_t*) memcpy(it, &message->messageType, sizeof(message->messageType));
    it = (uint8_t*) memcpy(it + sizeof(message->messageType), message->remainingLength, byteNum);
    it = (uint8_t*) memcpy(it + byteNum, &message->pid, 4);
    it = (uint8_t*) memcpy(it + 4, message->topic, message->topicLength);
    it = (uint8_t*) memcpy(it + message->topicLength, &message->qos, 1);

    return messageLength + byteNum + 1;
}

int mqtt_MqttSubackDeseralize(uint8_t* message, MqttSuback* const output){
    if(message[0] == SUBACK){
        memcpy(output, message, sizeof(MqttSuback));
        return 1;
    }
    return 0;
}

uint32_t mqtt_MqttUnsubscribeSerialize(uint8_t* buffer, MqttUnsubscribe* const message){
    uint8_t* it = buffer;

    uint8_t remainingLength [4];
    uint32_t messageLength = sizeof(message->pid) + sizeof(message->length) + message->length;

    uint8_t byteNum = mqtt_calculateRemainingLength(messageLength, remainingLength);

    it = (uint8_t*) memcpy(it, &message->messageType, sizeof(message->messageType));
    it = (uint8_t*) memcpy(it + sizeof(message->messageType), message->remainingLength, byteNum);
    it = (uint8_t*) memcpy(it + byteNum, &message->pid, 4);
    it = (uint8_t*) memcpy(it + 4, message->topic, message->length);

    return messageLength + byteNum + 1;
}

int mqtt_MqttUnsubackDeseralize(uint8_t* message, MqttUnsuback* const output){
    if(message[0] == UNSUBACK){
        memcpy(output, message, sizeof(MqttUnsuback));
        return 1;
    }
    return 0;
}

uint32_t mqtt_MqttPingreqSerialize(uint8_t* buffer, const MqttPingreq* const message){
    memcpy(buffer, message, sizeof(MqttPingreq));
    return sizeof(MqttPingreq);
}

int mqtt_MqttPingrespDeserialize(uint8_t* message, MqttPingresp* const output){
    if(message[0] == PINGRESP){
        memcpy(output, message, sizeof(MqttPingresp));
        return 1;
    }
    return 0;
}
