#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdint.h>

extern void* (*mqtt_allocator) (size_t);
extern void  (*mqtt_deallocator) (void*);

typedef enum {NO_ACK = 0, ACK = 1, TWO_STEP_ACK} MqttQos;

bool mqtt_connect(const char* const url, const char* const clientId, const char* const userName, const char* const password);
void mqtt_disconnect();

bool mqtt_publish(char* topic, char* message);
bool mqtt_subscribe(char* topic, MqttQos qos);

bool mqtt_unsubscribe(char* topic);


#endif // MQTT_CLIENT_H
