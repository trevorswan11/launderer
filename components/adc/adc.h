#pragma once

#include <stdint.h>

typedef struct {
    int bitwidth;
    int atten;
    int channel;
    int unit;
} analog_in;

int analog_in_init(analog_in* analog, uint8_t pin);
int analog_in_read(analog_in* analog, int* value);
