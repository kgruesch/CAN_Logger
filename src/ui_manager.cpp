#include <Arduino.h>
#include <ArduinoJson.h>
#include <string>
#include <WebSocketsServer.h>
#include "drive_mode.h"
#include "ui_manager.h"
#include "CAN_manager.h"
#include "globals.h"
#include "connection_manager.h"
#include "data_container.h"


String jsonString;
void getSettings() {
    settings.startType = readStartModes(startTypeAddr);
    settings.startMode = readStartModes(startModeAddr);
    settings.shiftLightRPM = readStartModes(shiftLightAddr);
    settings.oil_warning_temp = readStartModes(oil_warnAddr);
    settings.coolant_warning_temp = readStartModes(coolant_warnAddr);
    settings.tireType = readStartModes(tireTypeAddr);
}
void sendJSON()
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();
  String _mode = mode2string(CANdata.modeData);
  switch (CANdata.gear)
    {
      case -3:
        CANdata.gearString = ("N");
        break;

      case -2:  
        CANdata.gearString = ("R");
        break; 

      case -1 ... 0:  
        CANdata.gearString = ("P");
        break;

      case 1 ... 8:
        CANdata.gearString = String(CANdata.gear);
        break;

      default:
        CANdata.gearString = ("P");
        break;
    }
  //// Data////////
  object["RPM"]           = CANdata.tachometer;
  object["Gear"]          = CANdata.gearString;
  object["oil_temp"]      = CANdata.oil_temp;
  object["water_temp"]    = CANdata.water_temp;
  object["gearbox_temp"]  = CANdata.gearbox_temp;
  object["speed"]         = CANdata.speed; 
  object["driveMode"]     = _mode;
  object["ethContent"]    = CANdata.ethContent;
  object["fuel_temp"]     = CANdata.fuelTemp;
  object["inlineAccel"]   = CANdata.accel_y;
  object["lateralAccel"]  = CANdata.accel_x;
  object["throttle"]      = CANdata.throttle;
  object["brake"]         = CANdata.brake;
  object["boost"]         = CANdata.boost;
  //// Alerts////////
  object["o_warning"]     = alerts.oilWarning;
  object["c_warning"]     = alerts.coolWarning;
  object["coldEngine"]    = alerts.coldEngine;
  object["remind"]        = alerts.shiftReminder;
  object["shiftLight"]    = alerts.shiftLight;
  object["lightsOn"]      = alerts.lightsOn;
  object["DSC_alert"]     = flags.DSC_switch;
  //// Settings////////
  object["startMode"]     = settings.startMode;
  object["startType"]     = settings.startType;
  object["startMode"]     = mode2string(settings.startMode);
  object["shiftRPM"]      = settings.shiftLightRPM;
  object["OWT"]           = settings.oil_warning_temp;
  object["CWT"]           = settings.coolant_warning_temp;
  object["reminder_sec"]  = settings.shift_reminder_ms / 1000;
  object["tire"]          = settings.tireType;

  serializeJson(doc, jsonString);    // serialize the object and save teh result to teh string variable.
  Serial.println(jsonString);         // print the string for debugging.
  jsonString = "";                    // clear the String.
}