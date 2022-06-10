#include "mbed.h"

// Trains
// Fowards
#define TRAIN1   0x01
#define TRAIN2   0x02
#define FW05     0x62
#define FW30     0x64
#define FW50     0x68
#define FW75     0x6B
#define FW100    0x6F
#define FWSTOP   0x60
#define FWESTOP  0x61

//Backwards
#define BW50     0x48
#define BW75     0x4B
#define BW100    0x4F
#define BWSTOP   0x40
#define BWESTOP  0x41

//Lights
#define LIGHTON  0X90
#define LIGHTOFF 0x91

// Switches
#define SWITCHES 0x06
#define SWITCH1  0x81
#define SWITCH2  0x82
#define SWITCH3  0x84
#define SWITCH4  0x88
#define SWITCHI  0x80

