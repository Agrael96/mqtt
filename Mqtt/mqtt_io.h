#ifndef MQTT_IO_H
#define MQTT_IO_H

#include <stdint.h>
#include <stdbool.h>

void mqtt_send(const uint8_t* const message, uint32_t messageLength);
bool mqtt_receive(uint8_t* const receiveBuffer, uint32_t timeout);
bool mqtt_tcp_connect(const char* const url);
void mqtt_tcp_disconnect();

#endif // MQTT_IO_H
