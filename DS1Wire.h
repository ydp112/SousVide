#ifndef __DS_1_WIRE__
#define __DS_1_WIRE__
#include <stdint.h>
#include "mbed.h"

int Reset(DigitalInOut& pin);

void WriteBit(DigitalInOut& pin, uint32_t bit);
uint32_t ReadBit(DigitalInOut& pin);

void WriteByte(DigitalInOut& pin, uint32_t byte);
uint32_t ReadByte(DigitalInOut& pin);


#endif