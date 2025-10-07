#include <Arduino.h>
#include "EEPROM.h"
#include <vector>
#include "drive_mode.h"
#include "ui_manager.h"
#include "CAN_manager.h"
#include "globals.h"
#include "connection_manager.h"
#include "data_container.h"

uint8_t modes[] = {ECO, COMFORT, SPORT, SPORT_PLUS, TRAC, DSC_OFF};

void setDriveMode(int targetMode) 
{
  switch (targetMode)
  {
    case ECO:
    ///press down once
    GPIO_OUTPUT_SET(modeDown, 1);
    neopixelWrite(LED, 0, 0, 128);
    delay(shortPress);
    GPIO_OUTPUT_SET(modeDown, 0);
    neopixelWrite(LED, 0, 0, 0);
    break;

    case COMFORT: //do nothing, already in comfort
    break;

    case SPORT:
    // press up once
    GPIO_OUTPUT_SET(modeUp, 1);
    neopixelWrite(LED, 128, 0, 0);
    delay(shortPress);
    GPIO_OUTPUT_SET(modeUp, 0);
    neopixelWrite(LED, 0, 0, 0);
    break;

    case SPORT_PLUS:
    // press up twice
    for (int i = 0; i < 2; i++)
    {
      GPIO_OUTPUT_SET(modeUp, 1);
      neopixelWrite(LED, 0, 128, 128);
      delay(shortPress);
      GPIO_OUTPUT_SET(modeUp, 0);
      neopixelWrite(LED, 0, 0, 0);
      delay(shortPress);
    }
    break;

    case TRAC:
    // press DSC once
      GPIO_OUTPUT_SET(DSC, 1);
      neopixelWrite(LED, 128, 0, 0);
      delay(shortPress);
      GPIO_OUTPUT_SET(DSC, 0);
      neopixelWrite(LED, 0, 0, 0);
    break;

    case DSC_OFF:
    // press and hold dsc for 5 sec
      GPIO_OUTPUT_SET(DSC, 1);
      neopixelWrite(LED, 128, 128, 0);
      delay(longPress);
      GPIO_OUTPUT_SET(DSC, 0);
      neopixelWrite(LED, OFF);
    break;
  }
}


void string2mode(String modeIn, int &modeOut){
  modeOut = modeIn=="ECO"? ECO:
            modeIn == "Comfort"? COMFORT:
            modeIn == "Sport"? SPORT:
            modeIn == "Sport +"? SPORT_PLUS:
            modeIn == "Traction"? TRAC : DSC_OFF;
  
}

String mode2string(int &modeData) {
  if (modeData != -1) {}
  String mode = modeData==ECO? "ECO":
                modeData == COMFORT? "Comfort":
                modeData == SPORT? "Sport":
                modeData == SPORT_PLUS? "Sport +":
                modeData == TRAC? "Traction":"DSC Off";
  return mode;
}

bool writeToEEPROM(int addr, int data)
{
  EEPROM.put(addr, data);
  bool result = EEPROM.commit();
  delay(50);
  return result;
}

void InitEEPROM(void)
{
  if (!EEPROM.begin(EEPROM_SIZE)) 
    {
      Serial.println("failed to initialize EEPROM");
      neopixelWrite(21, 0, 128, 0); //red light if EEPROM fails to initialize
      delay(1000);
    }
}

int readStartModes(int addr)
{
    int data = EEPROM.readShort(addr);
    return (data);
}

void printStartInfo(int startModeType, int StartMode)
{
  Serial.print("Startup Mode Type: ");
  (startModeType == 1) ? Serial.println("Start in selected mode") : Serial.println("Start from last mode");
  Serial.print("Start Up Drive Mode: ");
  switch (StartMode)
  {
    case ECO:
        Serial.println("ECO");
        break;
    case COMFORT:
        Serial.println("Comfort");
        break;
    case SPORT:
        Serial.println("Sport");
        break;
    case SPORT_PLUS:
        Serial.println("Sport +");
        break;
    case TRAC:
        Serial.println("Traction");
        break;
    case DSC_OFF:
        Serial.println("DSC Off");
        break;
  }
}

void checkModeToggle () {

  if (CANdata.ToggleUp && !prevData.lastUpStatus) { // rising edge of up toggle triggered
    Serial.println("Up toggle triggered");
    prevData.lastUpStatus = true;
  }

  if (!CANdata.ToggleUp && prevData.lastUpStatus && !CANdata.ToggleUpUp) { // falling edge of toggle up triggered and up up isn't triggered
    prevData.lastUpStatus = false;
    switch (CANdata.modeData) {
      case ECO:
        GPIO_OUTPUT_SET(modeUp, 1);
        delay(shortPress);
        GPIO_OUTPUT_SET(modeUp, 0);
        break;
      
      case COMFORT:
        GPIO_OUTPUT_SET(modeUp, 1);
        delay(shortPress);
        GPIO_OUTPUT_SET(modeUp, 0);
        break;
      
      case SPORT:
      for (int i = 0; i < 2; i++)
      {
        GPIO_OUTPUT_SET(modeUp, 1);
        delay(shortPress);
        GPIO_OUTPUT_SET(modeUp, 0);
        delay(shortPress);
      }
        break;
      
      case SPORT_PLUS:
        GPIO_OUTPUT_SET(DSC, 1);        
        delay(shortPress);
        GPIO_OUTPUT_SET(DSC, 0);
        break;
      
      case TRAC:
        GPIO_OUTPUT_SET(DSC, 1);
        flags.DSC_switch = true;
        sendJSON();
        delay(longPress);
        GPIO_OUTPUT_SET(DSC, 0);
        flags.DSC_switch = false;
        sendJSON();
        break;

      default:
        break;
      
      }
      return;
    }

  else if (!CANdata.ToggleUp && prevData.lastUpStatus && CANdata.ToggleUpUp) { //up up goes directly to DSC off
      prevData.lastUpStatus = false;
      flags.DSC_switch = true;
      GPIO_OUTPUT_SET(DSC, 1);
      Serial.print("DSC switch triggered");
      sendJSON();
      delay(longPress);
      GPIO_OUTPUT_SET(DSC, 0);
      flags.DSC_switch = false;
      sendJSON();
      return;
  }
  
  if (CANdata.ToggleDown && !prevData.lastDownStatus) { // rising edge of down toggle triggered
    Serial.println("Down toggle triggered");
    prevData.lastDownStatus = true;
    switch (CANdata.modeData)
    {
      case ECO:
        // do nothing
        break;

      case COMFORT:
        GPIO_OUTPUT_SET(modeDown, 1);
        delay(shortPress);
        GPIO_OUTPUT_SET(modeDown, 0);
        break;
      
      case SPORT:
        GPIO_OUTPUT_SET(modeDown, 1);
        delay(shortPress);
        GPIO_OUTPUT_SET(modeDown, 0);
        break;
      
      case SPORT_PLUS:
        GPIO_OUTPUT_SET(modeDown, 1);
        delay(shortPress);
        GPIO_OUTPUT_SET(modeDown, 0);
        break;
      
      case TRAC:
        GPIO_OUTPUT_SET(modeDown, 1);
        delay(shortPress);
        GPIO_OUTPUT_SET(modeDown, 0);
        delay(shortPress);
        for (int i = 0; i < 4; i++) // Car goes into ECO from Trac or DSC when mode down is pressed- Switch to Sport+
        {
          GPIO_OUTPUT_SET(modeUp, 1);
          delay(shortPress);
          GPIO_OUTPUT_SET(modeUp, 0);
          delay(shortPress);
        }
        break;

        case DSC_OFF:
        GPIO_OUTPUT_SET(modeDown, 1);
        delay(shortPress);
        GPIO_OUTPUT_SET(modeDown, 0);
        for (int i = 0; i < 5; i++) // Car goes into ECO from Trac or DSC when mode down is pressed- Switch to TRAC
        {
          GPIO_OUTPUT_SET(modeUp, 1);
          delay(shortPress);
          GPIO_OUTPUT_SET(modeUp, 0);
          delay(shortPress);
        }
        break;
  }
      
  }
  else if (!CANdata.ToggleDown && prevData.lastDownStatus) { // falling edge of down toggle triggered
    prevData.lastDownStatus = false;
  }
  
}



