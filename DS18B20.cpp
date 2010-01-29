#include "DS18B20.h"
#include "DS1Wire.h"
#include "mbed.h"
#include <stdint.h>

// Device byte commands over 1-wire serial
enum COMMANDS { READ_ROM = 0x33, CONVERT = 0x44, READ_SCRATCHPAD = 0xBE,  SKIP_ROM = 0xCC };

// device onboard register layout
typedef struct {
    uint8_t    LSB;
    uint8_t    MSB;
    uint8_t    Th;
    uint8_t    Tl;
    uint8_t    config;
    uint8_t    reserved0xFF;
    uint8_t    reserved0xCH;
    uint8_t    reserved0x10;
    uint8_t    CRC;
} ScratchPad_t;


DigitalOut conversionInProgress(LED4);  // conversion in progress
DigitalOut resetFailure(LED1);          // for error reporting
extern DigitalInOut sensor;     // sensor pin

static void inError() {
    while (1) {
        resetFailure = !resetFailure;
        wait(0.2);
    }
}

void DoConversion() {
    if (Reset(sensor) != 0) {
        inError();
    } else {
        conversionInProgress = 1;       // led on
        WriteByte(sensor, SKIP_ROM);            // Skip ROM
        WriteByte(sensor, CONVERT);             // Convert
        while (ReadBit(sensor) == 0) {
            // wait for conversion to complete
        }
        conversionInProgress = 0;       // led off
    }
}

uint32_t GetTemperature() {
    uint32_t result = 0;
    if (Reset(sensor) != 0) {
        inError();
    } else {
        ScratchPad_t scratchpad;
        WriteByte(sensor, SKIP_ROM);    // Skip ROM
        WriteByte(sensor, READ_SCRATCHPAD);    // Read Scrachpad
        scratchpad.LSB = ReadByte(sensor);
        scratchpad.MSB = ReadByte(sensor);
        Reset(sensor);    // terminate read as we only want temperature
        result = ((scratchpad.MSB << 8) | scratchpad.LSB);
    }
    return result;
}

ROM_Code_t ReadROM() {
    ROM_Code_t ROM_Code;
    if (Reset(sensor) != 0) {
        inError();
    } else {
        
        WriteByte(sensor, READ_ROM);    // Read ROM
        for (uint32_t i = 0; i < 8; ++i) {
            ROM_Code.rom[i] = ReadByte(sensor);
        }
    }
    return ROM_Code;
}

// temperature is store as 7.4 fixed point format (assuming 12 bit conversion)
void displayTemperature(Serial& s) {
    DoConversion();
    uint32_t temp = GetTemperature();
    float f = (temp & 0x0F) * 0.0625;    // calculate .4 part
    f += (temp >> 4);    // add 7.0 part to it
    s.printf("Temp is %2.1fC\n\r", f);    // display in 2.1 format
}

