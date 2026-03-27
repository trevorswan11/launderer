#pragma once

#include <stdint.h>

typedef struct {
    struct hd44780* lcd_config;
    uint8_t         addr;
    uint8_t         sda;
    uint8_t         scl;
} lcd_t;

int  lcd_create(lcd_t* lcd, uint8_t addr, uint8_t sda, uint8_t scl);
void lcd_destroy(lcd_t* lcd);

int lcd_begin(lcd_t* lcd);
int lcd_set_cursor(lcd_t* lcd, uint8_t col, uint8_t row);
int lcd_print(lcd_t* lcd, const char* text);
int lcd_print_at(lcd_t* lcd, const char* text, uint8_t col, uint8_t row);
int lcd_clear(lcd_t* lcd);
