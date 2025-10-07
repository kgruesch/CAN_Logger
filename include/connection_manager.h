#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <Arduino.h>
#include <WiFi.h> 
#include <string>

bool initWifi();
void serialEvent();
void handleSerialInput(const String& jsonString);

#endif