#ifndef WIFI_OPS
#define WIFI_OPS
#include <WiFi.h>
#include <Arduino.h>


void wifi_begin();
void wifi_end();
byte wifi_check_for_client();

#endif