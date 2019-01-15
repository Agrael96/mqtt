#include "mqtt_io.h"

__attribute__((weak)) void mqtt_send(const uint8_t* const message, uint32_t messageLength){}
__attribute__((weak)) bool mqtt_receive(uint8_t* const receiveBuffer, uint32_t timeout){}
__attribute__((weak)) bool mqtt_tcp_connect(const char* const url){}
__attribute__((weak)) void mqtt_tcp_disconnect(){}
