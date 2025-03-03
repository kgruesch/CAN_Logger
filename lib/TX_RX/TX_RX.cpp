#include <Arduino.h>
#include "TX_RX.h"
#include <vector>
#include "drive_mode.h"

// WiFi.mode(WIFI_STA);
std::vector<uint8_t> writeAppData(std::vector<uint8_t>& appData)
{
    writeToEEPROM(startTypeAddr, appData[0]>>4);
    writeToEEPROM(isDisplayAddr, appData[0] &= 0x0F);
    writeToEEPROM(startModeAddr, appData[1]);
    writeToEEPROM(shiftLightAddr, appData[2]); 
    writeToEEPROM(dimmerAddr, appData[3]);
    writeToEEPROM(oil_warnAddr, appData[4]);
    writeToEEPROM(oil_dangerAddr, appData[5]);
    writeToEEPROM(reminderAddr, appData[6]); 
    writeToEEPROM(colorAddr, appData[7]);
    Serial.print("Values written:  ");
    for (int i=0; i< 8; i++)
    {
        Serial.print(appData[i]);
        Serial.print("\t");
    }
    Serial.println();
    return appData;
};

std::vector<u_int8_t> initAppData(std::vector<uint8_t>& appData)
{
    uint8_t startFromSetMode =      readStartModes(startTypeAddr);
    uint8_t isDisplay =             readStartModes(isDisplayAddr);
    uint8_t shiftRPM =              readStartModes(shiftLightAddr);
    uint8_t startupDriveMode =      readStartModes(startModeAddr);
    uint8_t oil_warning_temp =      readStartModes(oil_warnAddr);
        uint8_t oil_danger_temp =   readStartModes(oil_dangerAddr);
    uint8_t shift_reminder =        readStartModes(reminderAddr); 
    uint8_t RPMbar_temps =          readStartModes(colorAddr);

  
    appData[0] = startFromSetMode << 4 | (isDisplay &= 0x01);  
    appData[1] = startupDriveMode;
    appData[2] = shiftRPM >> 8; //split rpm into 2 bytes
    appData[3] = shiftRPM &= 0x00FF;
    appData[4] = oil_warning_temp;
    appData[5] = oil_danger_temp;
    appData[6] = shift_reminder;
    appData[7] = RPMbar_temps;
    Serial.print("Values read from EEPROM:  ");
    for (int i=0; i< 8; i++)
    {
        Serial.print(appData[i]);
        Serial.print("\t");
    }
    Serial.println();
    return appData;
};