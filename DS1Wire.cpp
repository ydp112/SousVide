#include "DS1Wire.h"
#include "mbed.h"
#include <stdint.h>

// Timing delay for 1-wire serial standard option
enum DELAY { A = 6, B = 64, C = 60, D = 10, E = 9, F = 55, G = 0, H = 480, I = 70, J = 410 };

   
int Reset(DigitalInOut& pin) {
    pin.output();
    pin = 0;    // drive bus low
    wait_us(H);
    pin.input(); // release bus
    wait_us(I);
    uint32_t result = pin;  // read bus value
    wait_us(J);
    return result;
}

void WriteBit(DigitalInOut& pin, uint32_t bit) {
    pin.output();
    if (bit) {
        pin = 0;        // drive bus low
        wait_us(A);        // delay A
        pin.input();      // release bus
        wait_us(B);        // delay B
    } else {
        pin = 0;    // drive bus low
        wait_us(C);    // delay C
        pin.input();  // release bus
        wait_us(D);    // delay D
    }
}

uint32_t ReadBit(DigitalInOut& pin) {
    uint32_t bit_value;
    pin.output();
    pin = 0;        // drive bus low
    wait_us(A);        // delay A
    pin.input();      // release bus
    wait_us(E);        // delay E
    bit_value  = pin;    // master sample bus
    wait_us(F);
    return bit_value;
}

void WriteByte(DigitalInOut& pin, uint32_t byte) {
    for (uint32_t bit = 0; bit < 8; ++bit) {
        WriteBit(pin, byte & 0x01); // lsb to msb
        byte >>= 1;    // right shift by 1-bit
    }
}

uint32_t ReadByte(DigitalInOut& pin) {
    uint32_t byte = 0;
    for (uint32_t bit = 0; bit < 8; ++bit) {
        byte |= (ReadBit(pin) << bit);    // Reads lsb to msb
    }
    return byte;
}