#ifndef CAN_DATA_H // 
#define CAN_DATA_H // 

#include "drive_mode.h"

// CAN frame IDs
#define rpmID       0x0A5 // 16 bit, 2 bytes, rpm = value / 4
#define temps       0x3F9 // water temp byte 4, oil temp byte 5: °C = value - 48
#define gearTemp    0x39A // 8 bit, Byte 1, °C = value / - 40
#define gearID      0x0F3 // 4 bit, byte 5, gear = value - 4
#define lightID     0x202 // 8 bit, byte 1, 0xFE if interior lighting is off, 0-253 brightness value otherwise
#define modeID      0x3A7 // 8bit, byte 2, hex values: 0x47= ECO, 0x13= COMFORT, 0x14= SPORT, 0x25= SPORT+, 0x11= TRAC, 0x26= DSC OFF
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

// Neopixel colors
#define red     "#fc0505"
#define green   "#0cf54a"
#define blue    "#0539f7"
#define white   "#f7f8fa"
#define black   "#000000"

struct VehicleData {
    int rpm {0};
    int gear {0};
    int water_temp {60};
    int oil_temp {60};
    int gearbox_temp {25};
    int illumination {0};
    int throttle {0};
    int modeData {0x19};
    int shift_Mode {0};
    int speed {0};
    int ethContent {0};
    int fuelTemp {0};
    float accel_y {0};
    float accel_x {0};
    float gyro {0};
    bool crzResume {false};
    String gearString {"P"};
    String driveMode {"Comfort"};
};

struct Settings {
    int oil_warning_temp        {120};
    int oil_danger_temp         {130};
    int shiftLightRPM           {5500};
    int startType               {true};
    int startMode               {SPORT};
    int shift_reminder_ms       {5000};
};

struct Alerts {
    bool shiftReminder {false};
    bool oilWarning {false};
    bool oilDanger {false};
    bool coldEngine {false};
    bool shiftLight {false};
    bool lightsOn {false};
};

struct PreviousData {
    int     last_gear {0},
            lastDriveMode {19},  
            lastRPM {0};
};

void CAN_start(int version);

void CANUpdate(int frameID, std::vector<int>& frame, VehicleData& data);

void setAlerts(VehicleData& data, Alerts& alerts, Settings& settings);

#endif