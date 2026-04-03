#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void* dev;
} load_cell_t;

// Channel_Factor
typedef enum {
    LOAD_CELL_GAIN_A_128 = 0,
    LOAD_CELL_GAIN_B_32,
    LOAD_CELL_GAIN_A_64,
} load_cell_gain_t;

typedef enum {
    LOAD_CELL_ON,
    LOAD_CELL_OFF,
} load_cell_power_t;

// For Adafruit Feather V2 DOUT == MOSI
int load_cell_create(load_cell_t* cell, uint8_t dout_pin, uint8_t sck_pin);
void load_cell_destroy(load_cell_t* cell);

int load_cell_set_power(load_cell_t* cell, load_cell_power_t power);
int load_cell_set_gain(load_cell_t* cell, load_cell_gain_t gain);

int load_cell_is_ready(load_cell_t* cell, bool* ready);
int load_cell_wait(load_cell_t* cell, size_t timeout_ms);

int load_cell_read_data(load_cell_t* cell, int32_t* data);
int load_cell_read_average(load_cell_t* cell, size_t times, int32_t* data);
