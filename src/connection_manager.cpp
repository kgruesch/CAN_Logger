#include <WiFi.h>       
#include <ArduinoJson.h> 
#include <string>
#include "drive_mode.h"
#include "ui_manager.h"
#include "CAN_manager.h"
#include "globals.h"
#include "connection_manager.h"
#include "data_container.h"
#include <esp_now.h>


extern "C" {
  #include "esp_wifi.h"
}

String inputBuffer = "";

// IPAddress static_ip(192, 168, 8, 112);
// IPAddress router_gateway(192, 168, 8, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress dns(8, 8, 8, 8);
// IPAddress ap_ip(192, 168, 10, 1);
// IPAddress ap_gateway(192, 168, 4, 1);

bool initWifi() {
  neopixelWrite(21, YELLOW);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_19_5dBm); // or comment this out entirely
  Serial.println("starting to WiFi");
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return false;
  }
  else return true;
  
  
 
  
  neopixelWrite(21, BLUE);
  return true;
}
void serialEvent() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') {
      handleSerialInput(inputBuffer);
      inputBuffer = "";
    } else {
      inputBuffer += c;
    }
  }
}
void handleSerialInput(const String& jsonString) {
  
  JsonDocument doc;
        deserializeJson(doc, jsonString);
        /* data structure:
        {"startType":1 or 0,
          "startMode":"Sport",
          "reminder":4000, (if value is 0, shift reminder is off)
          "danger":"128",
          "warning":"117",
          "shiftRPM":"5859"}*/
        settings.startType          = doc["sType"];
        settings.oil_warning_temp   = doc["OWT"];
        settings.coolant_warning_temp    = doc["CWT"];
        settings.shiftLightRPM      = doc["shftRPM"];
        int shift_reminder              = doc["rem_sec"];
        settings.shift_reminder_ms  = shift_reminder * 1000;
        settings.tireType           = doc["tireType"]; // 0 = summer, 1 = winter
        string2mode(doc["sMode"], settings.startMode);
        writeToEEPROM(startTypeAddr,  settings.startType);
        writeToEEPROM(startModeAddr,  settings.startMode);
        writeToEEPROM(shiftLightAddr, settings.shiftLightRPM);
        writeToEEPROM(oil_warnAddr,   settings.oil_warning_temp);
        writeToEEPROM(coolant_warnAddr, settings.coolant_warning_temp);
        writeToEEPROM(reminderAddr,   settings.shift_reminder_ms); 
        writeToEEPROM(tireTypeAddr,   settings.tireType);



}

