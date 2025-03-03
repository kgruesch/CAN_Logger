#ifndef DRIVE_MODE_H // 
#define DRIVE_MODE_H // 


#define startModeAddr         1
#define startTypeAddr         20
#define shiftLightAddr        40
#define dimmerAddr            60
#define oil_warnAddr          80
#define oil_dangerAddr        100
#define isDisplayAddr         120
#define reminderAddr          140
#define colorAddr             160
#define SSIDAddr              160
#define passwordAddr          180
#define modeUp                GPIO_NUM_4
#define modeDown              GPIO_NUM_5
#define DSC                   GPIO_NUM_6
#define PRESSED               1
#define NOT_PRESSED           0
#define longPress             5000
#define shortPress            200
#define ECO                   0x47
#define COMFORT               0x13
#define SPORT                 0x14
#define SPORT_PLUS            0x25
#define TRAC                  0x11
#define DSC_OFF               0x26
#define PARK                  0x20
#define REVERSE               0x40
#define DRIVE                 0x80
#define SPORTS                0x81
#define MANUAL                0x82
#define EEPROM_SIZE           320
#define LED                   21
#define BRIGHTNESS            100
//neopixel colors
#define RED                     0, 128, 0
#define GREEN                   128, 0, 0 
#define BLUE                    0, 0, 128
#define CYAN                    128, 0, 128
#define PURPLE                  0, 128, 128
#define YELLOW                  128, 128, 0
#define WHITE                   128, 128, 128
#define OFF                     0, 0, 0


void setDriveMode(int targetMode);

void string2mode(String modeIn, int &modeOut);

void InitEEPROM(void);

int readStartModes(int addr);

bool writeToEEPROM(int addr, int data);

void printStartInfo(int startModeType, int StartMode);

String mode2string(int &modeData);

#endif

//0x3A7- byte 2-5:  ECO =       0A 00 47 11, 
//                  Comfort =   00 00 13 11, 
//                  sport =     50 04 14 12, 
//                  sport+ =    5B 14 25 22, 
//                  TRAC =      04 10 11 21, 
//                  dsc off =   5D 47 26 42

//    0X189 BYTE 2 01 = CRUISE ON PRESSED, 10 = RESUME PRESSED
//          BYET 3 10 = SET, 01 = PADDLE UP 1/2, 02 = PADDLE UP FULL, 05 = PADDLE DOWN 1/2, 06 = PADDLE DOWN FULL
