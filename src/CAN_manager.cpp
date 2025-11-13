#include <Arduino.h>
#include <vector>
#include "HWCDC.h"
#include "driver/twai.h"
#include <driver/gpio.h>
#include "drive_mode.h"
#include "ui_manager.h"
#include "CAN_manager.h"
#include "globals.h"
#include "connection_manager.h"
#include "data_container.h"



void CAN_start(int version) // arg is board version, some pins change between the 2
{   gpio_num_t GPIO_TX;
    gpio_num_t GPIO_RX;
    gpio_num_t CAN_enable;
    GPIO_TX = (version == 1) ? GPIO_NUM_8 : GPIO_NUM_13;
    GPIO_RX = (version == 1) ? GPIO_NUM_9 : GPIO_NUM_12;
    CAN_enable = (version == 1) ? GPIO_NUM_7 : GPIO_NUM_1;
    GPIO_OUTPUT_SET(CAN_enable, 0);
    delay(150);
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_TX, GPIO_RX, TWAI_MODE_LISTEN_ONLY); 
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    twai_driver_install(&g_config, &t_config, &f_config);
    // Start TWAI driver
    twai_start(); 
}

void CANUpdate(uint32_t frameID,
               const uint8_t *frame,   // raw pointer to payload
               uint8_t         len,    // DLC (0–8)
               bool            extd,   // add the ext-ID flag while you’re here
               VehicleData&    data){    

    int x = 0;
    int calc = 0;
    int value = 0;
    uint16_t raw = 0;
    float val = 0;
    float brake2;
    

    switch (frameID)
    { 
        case gearID:
            data.gear = (frame[5] & 0x0F) - 4; // gear, 4-bit 50Hz
            x = frame[2] << 8 | (frame[1]);
            data.tachometer = round(x * 0.6288); // 16-bit value, 0-65536
            break;

        case temps:
            data.water_temp = frame[4] - 48; // 8-bit value, 0-255 water
            data.oil_temp = frame[5] - 48; // 8-bit value, 0-255 oil
            break;

        case rpmID: // 16bit 
            x = frame[6] << 8 | (frame[5]);
            data.rpm = round(x / 4); // 16-bit value, 0-65536
            data.torque_E = ((frame[3] << 8) | frame[2]) & 0x0FFF;
            data.torque_G = (((frame[4] << 8) | frame[3]) >> 4) & 0x0FFF;
            break;
            
        case gearTemp:
            data.gearbox_temp = frame[1] - 40; // 8-bit, 1Hz or on gear change
            break;

        case lightID: 
            data.illumination = frame[0]; //254 means interior lighting is off/daylight. Use full brightness
            break;

        case throttleID: //pedal position, 12 bit 100Hz
            calc = (frame[4] | (frame[5] << 8)); // 12-bit value, 0-4096
            calc &= 0x0FFF;
            data.throttle = calc/40; // convert to 0-100 u8_t value
            if (data.throttle == 1) data.throttle = 0;
            break;

        case modeID: //drive mode, 1Hz/subscription
            data.modeData = frame[4];
            break;

        case shiftMode:
            data.shift_Mode = frame[2];
            break;

        case speedID:
            data.speed = (frame[3] << 8 | frame[2]) / 100.0f;
            if (settings.tireType == 0) data.speed = data.speed * 0.96f;
            break;

        case cruiseID:
            data.crzResume =    (frame[2] == 0x10)? true : false;
            data.ToggleUp =     (frame[3] == 0x01)? true : false;
            data.ToggleUpUp =   (frame[3] == 0x02)? true : false;
            data.ToggleDown =   (frame[3] == 0x05)? true : false;
            break;

        case cruiseStatus:
            data.crzOn  = frame[2] != 0xE0? true : false; //0xE0 = off, 0x28 = on, 
            data.crzSet = frame[2] == 0x28? true : false;
            break;

        case ethanolID:
            data.ethContent = frame[0]; //%
            data.fuelTemp   = frame[1] - 40; //°C
            break;

        case accel_yID:
            raw = (frame[3] << 8 | frame[2]) ;
            val = raw / 500.0f - 65.0f;
            data.accel_y = val/9.81f; // in g
            if (fabsf(data.accel_y) < 0.02f) data.accel_y = 0.0f; // deadband filter
            break;

        case accel_xID:
            raw = (frame[3] << 8 | frame[2]); // in m/s²
            val = raw / 500.0f - 65.0f;
            data.accel_x = val/9.81f; // in g
            if (fabsf(data.accel_x) < 0.02f) data.accel_x = 0.0f; // deadband filter

            break;

        case gyroID:
            data.gyro = (frame[3] << 8 | frame[2]) * 0.005 - 163.84; // in degrees/s
            break;

        case brakeID:
            if (!data.brakeOn) { // brake is off, brake value should be 0
                data.brake = 0; 
                break;
            } 
            
            value = (frame[3] << 8 | frame[2]);
            data.brake = (31875-value) / 70;
            prevData.brake  = data.brake;
            data.brake = (data.brake <= 1) ? 0 : data.brake;
            break;

        case brakeOnID:
            data.brakeOn = (frame[6] == 0x30)? false : true;
            break;

        case Pressure_ID:
            data.ambient_mBar = frame[0] * 2 + 598;
            // Serial.println(data.ambient_mBar);
            break;
    }
    
}

void setAlerts(VehicleData& CANdata, Alerts& alerts, Settings& settings ) {
    alerts.oilWarning = (CANdata.oil_temp > settings.oil_warning_temp)? true : false;
    alerts.coolWarning = (CANdata.water_temp > settings.coolant_warning_temp)? true : false;
    alerts.coldEngine = (CANdata.oil_temp < 75)? true : false;
    alerts.shiftLight = (CANdata.rpm > settings.shiftLightRPM)? true : false;
    alerts.lightsOn = (CANdata.illumination == 0xFE)? false : true;
}


