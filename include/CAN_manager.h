#ifndef CAN_MANAGER_H // 
#define CAN_MANAGER_H // 

#include "drive_mode.h"
#include <vector>
#include "data_container.h"
#include "globals.h"



// CAN frame IDs
#define rpmID       0x0A5 // 16 bit, 2 bytes, rpm = value / 4
#define temps       0x3F9 // water temp byte 4, oil temp byte 5: °C = value - 48
#define gearTemp    0x39A // 8 bit, Byte 1, °C = value / - 40
#define gearID      0x0F3 // 4 bit, byte 5, gear = value - 4
#define lightID     0x202 // 8 bit, byte 1, 0xFE if interior lighting is off, 0-253 brightness value otherwise
#define modeID      0x3A7 // 8bit, byte 4, hex values: 0x47= ECO, 0x13= COMFORT, 0x14= SPORT, 0x25= SPORT+, 0x11= TRAC, 0x26= DSC OFF
#define parkID      0x197 // boolean, byte 3, 0xD5 if park button is pressed,0x0C if not pressed, 
#define throttleID  0x0D9 // 12 bits, mask 0x0FFF. Bytes 2-3: Throttle position, % = value / 4096; bytes 4-5: pedal position, % = value / 4096
#define cruiseID    0x189 // byte 2, 0x01 if cruise button is pressed, 0x10 if resume button is pressed, 0x00 if no button is pressed
#define shiftMode   0x3FD // 8bit, byte 2, hex values: 0x20= P, 0x40= R, 0x60= N, 0x80= D, 0x81= S, 0x82= M
#define speedID     0x1A1 // 16 bits, Bytes 2-3, MPH = value / 100
#define ethanolID   0x0EC // 8 bit, Byte 0: % = value, byte 1: °C = value - 40, byte 7: Sensor status- 0x00 = good, 0x01 = sensor fault
#define accel_yID   0x199 // 16 bits, Bytes 2-3, m/s² = value / 500 - 65
#define accel_xID   0x19A // 16 bits, Bytes 2-3, m/s² = value / 500 - 65
#define gyroID      0x19F // 16 bits, Bytes 2-3, deg/s = value / 200 - 163.84
#define IAT_ID      0x2C5 // 16 bits, Bytes 2-3, °C = value / 128 - 40
#define wheel_spdID 0x254 // 16 bits, Bytes 0-1: LR, bytes 2-3: RR, bytes 4-5: LF, bytes 6-7: RF; rad/s = value / 64 - 511.984
#define brakeID     0x0EF // 16 bits, Bytes 2-3: Brake % = (32000-value) / 90
#define brakeOnID   0x12F // 8 bit, byte 7: brake status. 0x30 = off, 0x33 = on
#define MAF_ID      0x08F // 16 bits, Bytes 2-3: MAF = unknown, byte 6: unkknown
#define cruiseStatus 0x289 // 8 bit, Byte 2: 0xE0 = Cruise Off, 0x28 = Cruise On, 0x2A = Cruise Set
#define Pressure_ID 0x3FB // 8 bits, Byte 0: Ambient pressure, mBar = value * 2 + 598


// Neopixel colors
#define red     "#fc0505"
#define green   "#0cf54a"
#define blue    "#0539f7"
#define white   "#f7f8fa"
#define black   "#000000"

void CAN_start(int version);

void CANUpdate(uint32_t id,
               const uint8_t* data,   // raw pointer to payload
               uint8_t         len,    // DLC (0–8)
               bool            extd,   // add the ext-ID flag while you’re here
               VehicleData&        out);

void setAlerts(VehicleData& data, Alerts& alerts, Settings& settings);

struct FrameLogging {
    std::vector<int> Frames {0x314, 0x302, 0x18D, 0x163, 0x08F, 0x0DC, 0x2C5, 0x2C4, 0x281, 0x0F3};
    std::vector<int> startByte {2, 2, 0, 6, 2, 3, 2, 5, 0, 1};
    std::vector<int> size {8, 16, 16, 16, 16, 16, 16, 8, 8, 16};
    std::vector<int> mask {false, false, true, false, false, false, false, false, false, false};
    std::vector<int> data {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};

#endif