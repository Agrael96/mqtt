#include "mqtt_client.h"
#include "mqtt_packets.h"
#include "mqtt_io.h"

#include <string.h>

static uint16_t PID = 1;

void* (*mqtt_allocator) (size_t);
void  (*mqtt_deallocator) (void*);

bool mqtt_connect(const char* const url, const char* const clientId, const char* const userName, const char* const password){
    bool returnValue = false;
    if(url && clientId){
        if(mqtt_tcp_connect(url)){
            MqttConnect connectMessage = MqttConnectDefaultInit();

            connectMessage.clientId = clientId;
            connectMessage.clientIdLength = strlen(clientId);

            if(userName && password){
                connectMessage.user = userName;
                connectMessage.userLength = strlen(userName);

                connectMessage.pass = password;
                connectMessage.passLength = strlen(password);
            }
            uint32_t payloadSize = connectMessage.clientIdLength + connectMessage.userLength + connectMessage.passLength;
            uint8_t* buffer = (uint8_t*)mqtt_allocator(payloadSize + 50);
            if(buffer){
                uint32_t messageSize = mqtt_MqttConnectSerialize(buffer, &connectMessage);
                mqtt_send(buffer, messageSize);

                uint8_t receiveBuffer[4];
                if(mqtt_receive(receiveBuffer, 300)){
                    MqttConnack responce;
                    if(mqtt_MqttConnackDeserialize(receiveBuffer, &responce)){
                        if(responce.returnCode == ACCEPTED){
                            returnValue = true;
                        }
                    }

                }
                mqtt_deallocator(buffer);
            }
        }
    }
    return returnValue;
}


void mqtt_disconnect(){
    MqttDisconnect disconnectMessage = MqttDisconnectDefualtInit();
    uint8_t buffer [2];
    mqtt_MqttDisconnectSerialize(buffer, &disconnectMessage);
    mqtt_send(buffer, 2);
    mqtt_tcp_disconnect();
}

bool mqtt_publish(char* topic, char* message){
    bool returnValue = false;
    if(topic && message){
        MqttPublish publishMessage = MqttPublishDefualtInit();
        publishMessage.topic = topic;
        publishMessage.topicLength = strlen(topic);
        publishMessage.payload = message;
        publishMessage.pid = PID;
        publishMessage.retain = 1;
        publishMessage.dup = 0;
        publishMessage.qos = 1;

        uint8_t* buffer = (uint8_t*) mqtt_allocator(strlen(message) + strlen(topic) + 10);
        if(buffer){
            uint32_t messageSize = mqtt_MqttPublishSerialize(buffer, &publishMessage);
            mqtt_send(buffer, messageSize);
            PID++;
            uint8_t receiveBuffer[4];
            if(mqtt_receive(receiveBuffer, 300)){
                MqttPuback puback;
                if(mqtt_MqttPubackDeserialize(receiveBuffer, &puback)){
                    if(puback.pid == (--PID)){
                        returnValue = true;
                    }
                }
            }
        }
    }
    return returnValue;
}

bool mqtt_subscribe(char* topic, MqttQos qos){
    bool returnValue = false;
    if(topic){
        MqttSubscribe subscribeMessage = MqttSubscribeDefualtInit();
        subscribeMessage.topic = topic;
        subscribeMessage.topicLength = strlen(topic);
        subscribeMessage.pid = PID;
        subscribeMessage.qos = qos;

        uint8_t* buffer = (uint8_t*)mqtt_allocator(15 + strlen(topic));
        if(buffer){
            uint32_t messageSize = mqtt_MqttSubscribeSerialize(buffer, &subscribeMessage);
            mqtt_send(buffer, messageSize);
            PID++;
            uint8_t receiveBuffer[5];
            if(mqtt_receive(receiveBuffer, 300)){
                MqttSuback suback;
                if(mqtt_MqttSubackDeseralize(receiveBuffer, &suback)){
                    if((suback.pid == (--PID)) && (suback.returnCode != 0x80)){
                        returnValue = true;
                    }
                }
            }
            mqtt_deallocator(buffer);
        }
    }
    return returnValue;
}

bool mqtt_unsubscribe(char* topic){
    bool returnValue = false;
    if(topic){
        MqttUnsubscribe unsubscribeMessage = MqttUnsubscribeDefualtInit();
        unsubscribeMessage.topic  = topic;
        unsubscribeMessage.length = strlen(topic);
        unsubscribeMessage.pid = PID;

        uint8_t* buffer = (uint8_t*)mqtt_allocator(15 + strlen(topic));
        if(buffer){
            uint32_t messageSize = mqtt_MqttUnsubscribeSerialize(buffer, &unsubscribeMessage);
            mqtt_send(buffer, messageSize);
            PID++;
            uint8_t receiveBuffer[4];
            if(mqtt_receive(receiveBuffer, 300)){
                MqttUnsuback unsuback;
                if(mqtt_MqttUnsubackDeseralize(receiveBuffer, &unsuback)){
                    if(unsuback.pid == (--PID)){
                        returnValue = true;
                    }
                }
            }
            mqtt_deallocator(buffer);
        }

    }
    return returnValue;
}
