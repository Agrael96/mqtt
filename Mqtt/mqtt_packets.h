#ifndef MQTTPACKETS_H
#define MQTTPACKETS_H

#include <stdint.h>

enum MqttMessageType {
    CONNECT     = 0x10,
    CONNACK     = 0x20,
    PUBLISH     = 0x03,
    PUBACK      = 0x40,
    PUBREC      = 0x50,
    PUBREL      = 0x62,
    PUBCOMP     = 0x70,
    SUBSCRIBE   = 0x82,
    SUBACK      = 0x90,
    UNSUBSCRIBE = 0xA2,
    UNSUBACK    = 0xB0,
    PINGREQ     = 0xC0,
    PINGRESP    = 0xD0,
    DISCONNECT  = 0xE0,
};

enum MqttConnectReturnCode {
    ACCEPTED         = 0x00,
    PROTOCOL_FAILED  = 0x01,
    CLIENT_ID_FAILED = 0x02,
    SERVER_FAILED    = 0x03,
    USER_PASS_FAILED = 0x04,
    AUTH_FAILED      = 0x05,
};

enum MqttSubscribeReturnCode {
    SUCCESS_QOS_0 = 0x00,
    SUCCESS_QOS_1 = 0x01,
    SUCCESS_QOS_2 = 0x02,
    FAILURE = 0x80
};

struct MqttConnect {
    uint8_t messageType;
    uint8_t* remainingLength;
    uint16_t length;
    char protocol [4];
    uint8_t protocolLevel;
    uint8_t connectFlag;
    uint16_t keepAlive;
    uint16_t clientIdLength;
    const char* clientId;
    uint16_t userLength;
    const char* user;
    uint16_t passLength;
    const char* pass;
};
#define MqttConnectDefaultInit() {CONNECT, NULL, 0x0004, {'M', 'Q', 'T', 'T'}, 0x04, 0x00, 10, 0, NULL, 0, NULL, 0, NULL};

struct MqttConnack {
    uint8_t messageType;
    uint8_t remainingLength;
    uint8_t flags;
    uint8_t returnCode;
};

struct MqttPublish {
    uint8_t messageType : 4;
    uint8_t dup : 1;
    uint8_t qos : 2;
    uint8_t retain : 1;
    uint8_t* remainingLength;
    uint16_t topicLength;
    char* topic;
    uint16_t pid;
    char* payload;
};
#define MqttPublishDefualtInit() {PUBLISH, 0, 0, 0, NULL, 0, NULL, 0, NULL}

struct MqttPuback {
    uint8_t messageType;
    uint8_t remainingLength;
    uint16_t pid;
};
#define MqttPubackDefualtInit() {PUBACK, 0x02, 0}

struct MqttPubrel {
    uint8_t messageType;
    uint8_t remainingLength;
    uint16_t pid;
};
#define MqttPubrelDefualtInit() {PUBREL, 0x02, 0}

struct MqttPubcomp {
    uint8_t messageType;
    uint8_t remainingLength;
    uint16_t pid;
};
#define MqttPubcompDefualtInit() {PUBCOMP, 0x02, 0}

struct MqttSubscribe {
    uint8_t messageType;
    uint8_t* remainingLength;
    uint16_t pid;
    uint16_t topicLength;
    char* topic;
    uint8_t qos;
};
#define MqttSubscribeDefualtInit() {SUBSCRIBE, NULL, 0, 0, NULL, 0}

struct MqttSuback {
    uint8_t messageType;
    uint8_t remainingLength;
    uint16_t pid;
    uint8_t returnCode;
};

struct MqttUnsubscribe {
    uint8_t messageType;
    uint8_t* remainingLength;
    uint16_t pid;
    uint16_t length;
    char* topic;
};
#define MqttUnsubscribeDefualtInit() {UNSUBSCRIBE, NULL, 0, 0, NULL}

struct MqttUnsuback {
    uint8_t messageType;
    uint8_t remainingLength;
    uint16_t pid;
};

struct MqttPingreq {
    uint8_t messageType;
    uint8_t remainingLength;
};
#define MqttPingreqDefualtInit() {PINGREQ, 0x00}


struct MqttPingresp {
    uint8_t messageType;
    uint8_t remainingLength;
};

struct MqttDisconnect {
    uint8_t messageType;
    uint8_t remainingLength;
};
#define MqttDisconnectDefualtInit() {DISCONNECT, 0x00}


uint32_t mqtt_MqttConnectSerialize(uint8_t* const buffer, MqttConnect* const message);
uint32_t mqtt_MqttDisconnectSerialize(uint8_t* const buffer, const MqttDisconnect* const message);

int mqtt_MqttConnackDeserialize(uint8_t* message, MqttConnack* const output);

uint32_t mqtt_MqttPublishSerialize(uint8_t* buffer, MqttPublish* const message);
int mqtt_MqttPublishDeserialize(uint8_t* message, MqttPublish* const output);

uint32_t mqtt_MqttPubackSerialize(uint8_t* buffer, const MqttPuback* const message);
int mqtt_MqttPubackDeserialize(uint8_t* message, MqttPuback* const output);

uint32_t mqtt_MqttPubrelSerialize(uint8_t* buffer, const MqttPubrel* const message);
int mqtt_MqttPubrelDeserialize(uint8_t* message, MqttPubrel* const output);

uint32_t mqtt_MqttPubcompSerialize(uint8_t* buffer, const MqttPubcomp* const message);
int mqtt_MqttPubcompDeserialize(uint8_t* message, MqttPubcomp* const output);

uint32_t mqtt_MqttSubscribeSerialize(uint8_t* buffer, MqttSubscribe* const message);

int mqtt_MqttSubackDeseralize(uint8_t* message, MqttSuback* const output);

uint32_t mqtt_MqttUnsubscribeSerialize(uint8_t* buffer, MqttUnsubscribe* const message);

int mqtt_MqttUnsubackDeseralize(uint8_t* message, MqttUnsuback* const output);

uint32_t mqtt_MqttPingreqSerialize(uint8_t* buffer, const MqttPingreq* const message);

int mqtt_MqttPingrespDeserialize(uint8_t* message, MqttPingresp* const output);

#endif // MQTTPACKETS_H
