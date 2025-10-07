// #include <Arduino.h>
// #include "driver/twai.h"
// #include <vector>
// #include "EEPROM.h"
// #include "drive_mode.h"
// #include "logger.h"
// #include "TX_RX.h"
// #include "SocketCAN.h"
// #include <stdlib.h>
// // #include "ADS1X15.h"
// #include <ADS1115_lite.h>
// #include <Wire.h>
// #include <driver/gpio.h>

// #define ESP_SDA 8
// #define ESP_SCL 9

// int ID;
// Logging logger;
// bool frameData = false;
// bool header = false;
// unsigned long timer1 = 0;
// unsigned long timer2 = 0;
// unsigned long timer3 = 0;
// VehicleData CANdata;
// Settings settings;
// FrameLogging logFrames;
// // ADS1115 ADS(0x48);
// ADS1115_lite ADS(ADS1115_DEFAULT_ADDRESS); //Initializes wire library, sets private configuration variables to ADS1115 default(2.048V, 128SPS, Differential mode between  AIN0 and AIN1.  The Address parameter is not required if you want default

// //Setup some variables used to show how long the ADC conversion takes place
//     unsigned long starttime;
//     unsigned long endtime ;
//     int16_t Raw;

// float read_ADS(int channel) {
  
//   switch (channel) {
//     case 0:
//     ADS.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_0);
//     break;

//     case 1:
//     ADS.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_1);
//     break;

//     case 2:
//     ADS.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_2);
//     break;

//     case 3:
//     ADS.setMux(ADS1115_REG_CONFIG_MUX_SINGLE_3);
//     break;
//   }

//   // Serial.println("triggering conversion...");
//   starttime = micros(); //Record a start time for demonstration
//   ADS.triggerConversion(); //Start a conversion.  This immediatly returns
//   delayMicroseconds(1500);
//   Raw = ADS.getConversion(); //This polls the ADS1115 and wait for conversion to finish, THEN returns the value
//   endtime = micros();
//   // Serial.print("Conversion complete: "); Serial.print(Raw); Serial.print(",  "); Serial.print(endtime - starttime);  Serial.print("us");
//   // Serial.println("=======================");
  
//   return (6.14 * Raw / 32767) ;
// }

// void setup() {
//   neopixelWrite(21, OFF);
//   GPIO_OUTPUT_SET(GPIO_NUM_7, 1);
//   GPIO_OUTPUT_SET(GPIO_NUM_10, 1);
//   Serial.begin(230400);
//   // Wire.setPins(ESP_SDA, ESP_SCL);
//   // ADS.begin();
//   // ADS.setMode(1);
//   ADS.setGain(ADS1115_REG_CONFIG_PGA_6_144V);
//   ADS.setSampleRate(ADS1115_REG_CONFIG_DR_860SPS); 
//   // Serial.printf("ADS Mode: %d\n", ADS.getMode());  // Should print 1
//   // ADS.setDataRate(7);
//   // Serial.printf("Data rate set to: 0x%X\n", ADS.getDataRate());
//   if (!ADS.testConnection()) {
//     Serial.println("ADS1115 Connection failed"); //oh man...something is wrong
//     return;
//   }
//   Serial.println();
//   neopixelWrite(21, YELLOW);
//   // adcAttachPin(MAP_pin);
//   // adcAttachPin(IAT_pin);
//   delay(800);
//   Serial.println("Serial on");
//   Serial.print("log Rate set to: ");
//   Serial.print(settings.logRate);
//   Serial.println(" ms");
//   CAN_start(2); //switch TX/RX/enable based on board version, see socketCAN.cpp
//   Serial.println("CAN started");
//   neopixelWrite(21, BLUE);
//   delay(800);
// }

// void loop() 
// { 
//   // unsigned long timer2 = micros();
//   // long gainTime = micros() - timer2;
//   // timer2 = micros();
//   float MAP_volts = read_ADS(2);   // returns value in volts
//   // long mapReadTime = micros() - timer2;
//   // timer2 = micros();
//   float IAT_volts = read_ADS(1);   // returns value in volts5(1);
//   // long IATreadTime = micros() - timer2;

//   twai_message_t message;

//   /// Type * to log all frames, or type a frame ID in hex (not case sensitive,format "0F3" or "0f3" for example) to log that frame only
//   /// spacebar starts and stops the logger
//   if (twai_receive(&message, pdMS_TO_TICKS(1)) == ESP_OK) {
//     std::vector <int> CANframe(message.data_length_code);
//       for (int i = 0; i < message.data_length_code; i++) {
//         CANframe[i] = message.data[i];
//       }
//     CANUpdate(message.identifier, CANframe, CANdata);
      
//     if (Serial.available() > 0 ) {
//       int val = Serial.read();
//       Serial.println("Console input received");

//       if (val == 32) {//spacebar starts and stops the logger
//         logger.logger = !logger.logger; //toggle logger on space bar entry
//         logger.logger? Serial.println("Logger on") : Serial.println("Logger off");
//         delay(500);
//         logger.logger? neopixelWrite(21, GREEN) : neopixelWrite(21, OFF);
//         while (Serial.available() > 0 ) //empty the buffer
//         int trash = Serial.read();
//       }
//       else if (val == 42) { // type "*" to log all frames 
//         logger.ID_all = true;
//         Serial.print("Logging all frames");
//         delay(500);
//         // settings.logRate < 10? settings.logRate = 10 : settings.logRate;
//         while (Serial.available() > 0 ) //empty the buffer
//           int trash = Serial.read();
//       }
//       else if (val == 35) { // type "#" to toggle frame logging 
//         frameData = !frameData;
//         logger.ID_all = false;
//         header = false;
//         while (Serial.available() > 0 ) //empty the buffer
//         int trash = Serial.read();
//       }
//       else {
//         logger.ID_all = false;
//         logger.ID = loggerID(val, false); // Type a frame ID in hex to log that frame only
      
//       }
//     }
//     if (logger.logger && (message.identifier == logger.ID || logger.ID_all) && !frameData) {
//       // if (millis() - timer1 > 5) {
//       timer1 = millis();
//       double time = millis() / 10;
//       time = round(time);
//       time = time/100;
//       Serial.print(time);
//       Serial.print("  ");
//       Serial.print("ID: ");
//       Serial.print("0x");
//       Serial.print(message.identifier, HEX);
//       Serial.print("\t");
//       for(int i=0;i<8;i++) 
//       { 
//         Serial.print("  ");
//         if (i >= message.data_length_code) {
//           Serial.print("X");
//         }
//         else {
//           Serial.print(message.data[i], DEC);
//         }
        
//       }
//       Serial.print("  ");
  
//       Serial.print(MAP_volts);
//       Serial.print("  "); 

//       Serial.print(IAT_volts);
//       Serial.print("  ");

//       Serial.print(CANdata.rpm);
//       Serial.print("  ");
//       Serial.print(CANdata.throttle);
//       Serial.println();
//       }
//     // }
//     /////////////////////////////////////////////////////////////////////
//     //////// Logging specific frames together //////////////////////////
//     else if (frameData){
//       int numFrames = logFrames.Frames.size();
//       for (int i = 0; i < numFrames; i++) {
//         if (logFrames.Frames[i] == message.identifier) {
//           /// 16 bit frame
//           if (logFrames.size[i] == 16) {
//             int startByte = logFrames.startByte[i];
//             uint16_t data = message.data[startByte + 1] << 8 | message.data[startByte];
//             logFrames.data[i] = data;
//             // apply 12 bit mask if applicable
//             if (logFrames.mask[i]) {
//               logFrames.data[i] = data & 0x0FFF;
//             }
            
//           }
//           /// 8 bit frame
//           else {
//             logFrames.data[i] = message.data[logFrames.startByte[i]];
//           }            
//         }
//       }
//       if (millis() - timer2 > 93) {
//         timer2 = millis();
        
//         if (!header) {
//           Serial.print("Time");
//           Serial.print("  ");
//           for (int i = 0; i < numFrames; i++) {
//             Serial.print("0x");
//             Serial.print(logFrames.Frames[i], HEX);
//             Serial.print("-");
//             Serial.print(logFrames.startByte[i]);
//             Serial.print("  ");
//           }
//           Serial.print("MAP");
//           Serial.print("  ");
//           Serial.print("IAT");
//           Serial.print("  ");
//           Serial.print("RPM");
//           Serial.print("  ");
//           Serial.print("Thr");
//           Serial.print("  ");
//           Serial.println();
//           header = true;
//         }
//         double timestamp = millis();
//         Serial.print(timestamp/1000);
//         Serial.print("  ");
//         for (int i = 0; i < numFrames; i++) {
//           Serial.print(logFrames.data[i], DEC);
//           Serial.print("  ");
//         }
//         Serial.print("  ");
//         Serial.print(MAP_volts);
//         Serial.print("  ");
//         Serial.print(IAT_volts);
//         Serial.print("  ");
//         Serial.print(CANdata.rpm);
//         Serial.print("  ");
//         Serial.print(CANdata.throttle);
//         Serial.println(); 
//       }
//     }
//   }
// }


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

  if (Serial.available() > 0) {
    int val = Serial.read();
    if (val == 32) { //space bar
      logger.logger = !logger.logger;
      Serial.println(logger.logger ? "Logger on" : "Logger off");
      header = false;
      delay(500);
      neopixelWrite(21, logger.logger ? GREEN : OFF);
    } else if (val == 42) { // * key
      logger.ID_all = true;
      Serial.println("Logging all frames");
      delay(500);
      settings.logRate = max(settings.logRate, 10);
    } else if (val == 35) { // # key to log group of frames
      frameData = !frameData;
      logger.ID_all = false;
      header = false;
    } else { // type 3 digit frame ID to log it e.g.. 3FD or 3fd
      logger.ID_all = false;
      logger.ID = loggerID(val, false);
    }
    while (Serial.available() > 0) Serial.read(); // clear buffer
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
      Serial.println("MAP  IAT  RPM  Thr");
      header = true;
    }
    double timestamp = millis() / 1000.0;
    Serial.printf("%.3f  ", timestamp);
    for (int i = 0; i < numFrames; i++) Serial.printf("%d  ", logFrames.data[i]);
    Serial.printf("%.2f  %.2f  %d  %d\n", ESPNOWData.boost_mBar, ESPNOWData.IAT_volts, CANdata.rpm, CANdata.throttle);
    }
  }
      commsStarted = millis() - reconnectTimer > 500 ? false : commsStarted;
}
  
