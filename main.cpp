#include "mbed.h"
#include <stdint.h>
#include "DS18B20.h"

DigitalInOut sensor(p5);     // sensor connected to pin 5

Ticker timer;                // used for our microsec timing
Serial pc(USBTX, USBRX);     // serial comms over usb back to console

int main() {
    pc.printf("\n\r=====================================================\n\r");
    pc.printf("DS18B20 Configuration\n\r");
    sensor.mode(PullUp);
    
    ROM_Code_t ROM_Code = ReadROM();
    pc.printf("Family code: 0x%X\n\r", ROM_Code.BYTES.familyCode);
    pc.printf("Serial Number: ");
    for (uint32_t i = 6; i != 0; --i) {
        pc.printf("%02X%s", ROM_Code.BYTES.serialNo[i-1], (i != 1)?":":"\r\n");
    }
    pc.printf("CRC: 0x%X\r\n", ROM_Code.BYTES.CRC);
    
    pc.printf("\n\rRunning temperature conversion...\n\r");
    while (1) {
        displayTemperature(pc);
        wait(10);
    }
}

