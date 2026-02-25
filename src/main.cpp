#include <Arduino.h>
#include <WiFi.h>
#include "driver/twai.h"
#include <vector>
#include "EEPROM.h"
#include "drive_mode.h"
#include "ui_manager.h"
#include "CAN_manager.h"
#include "globals.h"
#include "connection_manager.h"
#include "data_container.h"
#include <stdlib.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "logger.h"


#include <driver/gpio.h>


// Globals
bool commsStarted = false;
unsigned long reconnectTimer = 0;
uint8_t macAddr[6];
int ID;
Logging logger;
bool frameData = false;
bool header = false;
unsigned long timer1 = 0;
unsigned long timer2 = 0;
unsigned long timer3 = 0;
VehicleData CANdata;
Settings settings;
Alerts alerts;
PreviousData prevData;
Flags flags;
Timers timers;
FrameLogging logFrames;

typedef struct ESPNOWData {
  float boost_mBar, IAT_volts, Oil_pres_kPa, aux_volts;
} struct_message;

struct_message ESPNOWData;


// Task Handles
TaskHandle_t CANReaderTask;
TaskHandle_t SensorTask;

void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(&ESPNOWData, data, sizeof(ESPNOWData));
  ESPNOWData.boost_mBar -= CANdata.ambient_mBar;
  commsStarted = true;
  neopixelWrite(21, BLUE);
  reconnectTimer = millis();
}

void setup() {
  neopixelWrite(21, OFF);
  GPIO_OUTPUT_SET(GPIO_NUM_7, 1);
  GPIO_OUTPUT_SET(GPIO_NUM_10, 1);
  Serial.begin(115200);

  
  // delay(800);
  Serial.println("Serial on");
  Serial.printf("log Rate set to: %d ms\n", settings.logRate);
  CAN_start(2);
  Serial.println("CAN started");
  neopixelWrite(21, BLUE);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); // Pick your channel
  esp_wifi_set_promiscuous(false);
  WiFi.disconnect();
  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress()); // This returns a String, e.g. "34:B7:DA:59:0A:E4"
  delay(100);

  if (esp_now_init() == ESP_OK) 
    Serial.println("ESP-NOW Initilized");
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t broadcastPeer = {};
  memset(broadcastPeer.peer_addr, 0xFF, 6);
  broadcastPeer.channel = 0;
  broadcastPeer.encrypt = false;

if (!esp_now_is_peer_exist(broadcastPeer.peer_addr)) {
  if (esp_now_add_peer(&broadcastPeer) == ESP_OK) {
    Serial.println("Broadcast peer added");
  } else {
    Serial.println("Failed to add broadcast peer");
  }
}
  
  WiFi.macAddress(macAddr);
}

void loop() {

  while (!commsStarted) {
    neopixelWrite(21, GREEN);
    uint8_t broadcastAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
    esp_err_t result = esp_now_send(broadcastAddr, (uint8_t*)&macAddr, sizeof(macAddr));
    commsStarted = true;
    if (result == ESP_OK) {
      Serial.println("Send MAC Address Success");
    } else {
       Serial.println("Send MAC Address Failed");
    }
    delay(50);
  }
  int input = handleSerialCommands();
  if (input == -1) {
    logger.logger = !logger.logger;
    Serial.printf("Logger %s\n", logger.logger? "On" : "Off");
    neopixelWrite(21, logger.logger? BLUE : OFF);
  }
  else if (input > 0) {
    logger.ID = input;
  }

  twai_message_t message;
  if (twai_receive(&message, pdMS_TO_TICKS(1)) == ESP_OK) {
    uint8_t payload[8];
    uint8_t len = message.data_length_code;
    memcpy(payload, message.data, len);
    CANUpdate(message.identifier, payload, len, message.extd, CANdata);
    setAlerts(CANdata, alerts, settings);
    static unsigned long lastCANTime = 0;
    lastCANTime = millis(); 
  }
  /// If logging all IDs, throttle printing to not crash terminal
  if (logger.logger && logger.ID_all && !frameData) {
    if (millis() - timer1 > 5) {
      timer1 = millis();
      double time = round(millis() / 10.0) / 100;
      Serial.printf("%.3f  ID: 0x%X\t", time, message.identifier);
      for (int i = 0; i < 8; i++) {
        Serial.print(i >= message.data_length_code ? " X" : String(message.data[i]));
        Serial.print("  ");
      }
      Serial.printf("%.3f  %.3f  %d  %d\n", ESPNOWData.boost_mBar, ESPNOWData.IAT_volts, CANdata.rpm, CANdata.throttle);
    }
  } 
      /// If logging single frame, print every time one is received///////
  else if (logger.logger && message.identifier == logger.ID && !logger.ID_all && !frameData) { 
    double time = round(millis() / 10.0) / 100;
    Serial.printf("%.3f  ID: 0x%X\t", time, message.identifier);
    for (int i = 0; i < 8; i++) {
      Serial.print(i >= message.data_length_code ? " X" : String(message.data[i]));
      Serial.print("  ");
    }
    Serial.printf("%.3f  %.3f  %d  %d\n", ESPNOWData.boost_mBar, ESPNOWData.IAT_volts, CANdata.rpm, CANdata.throttle);
  }
  else if (frameData) {
  int numFrames = logFrames.Frames.size();
  for (int i = 0; i < numFrames; i++) {
    if (logFrames.Frames[i] == message.identifier) {
      int startByte = logFrames.startByte[i];
      if (logFrames.size[i] == 16) {
        uint16_t data = message.data[startByte + 1] << 8 | message.data[startByte];
        logFrames.data[i] = logFrames.mask[i] ? data & 0x0FFF : data;
      } else {
        logFrames.data[i] = message.data[startByte];
      }
    }
  }
  if (millis() - timer2 > settings.logRate) {
    timer2 = millis();
    if (!header) {
      Serial.print("Time  ");
      for (int i = 0; i < numFrames; i++) {
        Serial.printf("0x%X-%d  ", logFrames.Frames[i], logFrames.startByte[i]);
      }
      Serial.println("MAP  IAT  RPM  Thr  Torque_E  Torque_G");
      header = true;
    }
    double timestamp = millis() / 1000.0;
    Serial.printf("%.3f  ", timestamp);
    for (int i = 0; i < numFrames; i++) Serial.printf("%d  ", logFrames.data[i]);
    Serial.printf("%.2f  %.2f  %d  %d  %d  %d\n", ESPNOWData.boost_mBar, ESPNOWData.IAT_volts, CANdata.rpm, CANdata.throttle, CANdata.torque_E, CANdata.torque_G);
    }
  }
      commsStarted = millis() - reconnectTimer > 500 ? false : commsStarted;
}
  