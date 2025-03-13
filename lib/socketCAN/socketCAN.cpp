#include <Arduino.h>
#include <vector>
#include "HWCDC.h"
#include "driver/twai.h"
#include <driver/gpio.h>
#include "socketCAN.h"
#include "TX_RX.h"
#include "drive_mode.h"

void CAN_start(int version) // arg is board version, some pins change between the 2
{   gpio_num_t GPIO_TX;
    gpio_num_t GPIO_RX;
    gpio_num_t CAN_enable;
    GPIO_TX = (version == 1) ? GPIO_NUM_8 : GPIO_NUM_13;
    GPIO_RX = (version == 1) ? GPIO_NUM_9 : GPIO_NUM_12;
    CAN_enable = (version == 1) ? GPIO_NUM_7 : GPIO_NUM_1;
    GPIO_OUTPUT_SET(CAN_enable, 0);
    delay(150);
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_TX, GPIO_RX, TWAI_MODE_NORMAL); //change to 12 tx, 13 rx
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    twai_driver_install(&g_config, &t_config, &f_config);
    // Start TWAI driver
    twai_start(); 
}

void CANUpdate(int frameID, std::vector<int>& frame, VehicleData& data)
{
    int x = 0;
    int calc = 0;
    switch (frameID)
    { 
        case gearID:
            data.gear = (frame[5] &= 0x0F) - 4; // gear, 4-bit 50Hz
            break;

        case temps:
            data.water_temp = frame[5] - 48; // 8-bit value, 0-255 water
            data.oil_temp = frame[4] - 48; // 8-bit value, 0-255 oil
            break;

        case rpmID: // 16bit 
                x = frame[6] << 8 | (frame[5]);
                data.rpm = round(x / 4); // 16-bit value, 0-65536
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
            break;

        case modeID: //drive mode, 1Hz/subscription
            data.modeData = frame[4];
            break;

        case shiftMode:
            data.shift_Mode = frame[2];
            break;

        case speedID:
            data.speed = (frame[3] << 8 | frame[2]) / 100;
            break;

        case cruiseID:
            data.crzResume = (frame[2] == 0x10)? true : false;
            break;

        case ethanolID:
            data.ethContent = frame[0]; //%
            data.fuelTemp = frame[1] - 40;
            break;

        case accel_yID:
            data.accel_y = (frame[3] << 8 | frame[2]) *0.008 - 65; // in m/s²
            break;

        case accel_xID:
            data.accel_x = (frame[3] << 8 | frame[2]) *0.008 - 65; // in m/s²
            break;

        case gyroID:
            data.gyro = (frame[3] << 8 | frame[2]) *0.005 - 163.84; // in degrees/s
    }
}

void setAlerts(VehicleData& CANdata, Alerts& alerts, Settings& settings ) {
    alerts.oilWarning = (CANdata.oil_temp > settings.oil_warning_temp)? true : false;
    alerts.oilDanger = (CANdata.oil_temp > settings.oil_danger_temp)? true : false;
    alerts.coldEngine = (CANdata.oil_temp < 75)? true : false;
    alerts.shiftLight = (CANdata.rpm > settings.shiftLightRPM)? true : false;
    alerts.lightsOn = (CANdata.illumination == 0xFE)? false : true;
}
