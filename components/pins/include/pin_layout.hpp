#pragma once

#include <cstdint>
#include <optional>
#include <utility>

#include <hal/adc_types.h>
#include <soc/soc_caps.h>

// Specific pin layout for the Adafruit ESP32 Feather V2
namespace dev {

using Pin = uint8_t;

namespace pin_layout {

constexpr uint8_t TX = 8;
constexpr uint8_t RX = 7;

constexpr uint8_t SDA = 22;
constexpr uint8_t SCL = 20;

constexpr uint8_t SS   = 33;
constexpr uint8_t MOSI = 19;
constexpr uint8_t MISO = 21;
constexpr uint8_t SCK  = 5;

constexpr uint8_t A0  = 26;
constexpr uint8_t A1  = 25;
constexpr uint8_t A2  = 34;
constexpr uint8_t A3  = 39;
constexpr uint8_t A4  = 36;
constexpr uint8_t A5  = 4;
constexpr uint8_t A6  = 14;
constexpr uint8_t A7  = 32;
constexpr uint8_t A8  = 15;
constexpr uint8_t A9  = 33;
constexpr uint8_t A10 = 27;
constexpr uint8_t A11 = 12;
constexpr uint8_t A12 = 13;
constexpr uint8_t A13 = 35;

// vbat measure
constexpr uint8_t BATT_MONITOR = 35;

// internal switch
constexpr uint8_t BUTTON = 38;

constexpr uint8_t LED_BUILTIN = 13;

// Neopixel
constexpr uint8_t PIN_NEOPIXEL   = 0;
constexpr uint8_t RGB_BUILTIN    = PIN_NEOPIXEL + SOC_GPIO_PIN_COUNT;
constexpr uint8_t RGB_BRIGHTNESS = 64;

// Neopixel & I2C power
constexpr uint8_t NEOPIXEL_I2C_POWER = 2;

constexpr uint8_t T0 = 4;
constexpr uint8_t T1 = 0;
constexpr uint8_t T2 = 2;
constexpr uint8_t T3 = 15;
constexpr uint8_t T4 = 13;
constexpr uint8_t T5 = 12;
constexpr uint8_t T6 = 14;
constexpr uint8_t T7 = 27;
constexpr uint8_t T8 = 33;
constexpr uint8_t T9 = 32;

constexpr uint8_t DAC1 = 25;
constexpr uint8_t DAC2 = 26;

// Attempts to map a GPIO pin number to its corresponding ADC channel and unit.
constexpr auto pin_to_adc(uint32_t pin) -> std::optional<std::pair<adc_unit_t, adc_channel_t>> {
    switch (pin) {
    case A0: return std::pair{ADC_UNIT_2, ADC_CHANNEL_9};
    case A1: return std::pair{ADC_UNIT_2, ADC_CHANNEL_8};
    case A2: return std::pair{ADC_UNIT_1, ADC_CHANNEL_6};
    case A3: return std::pair{ADC_UNIT_1, ADC_CHANNEL_3};
    case A4: return std::pair{ADC_UNIT_1, ADC_CHANNEL_0};
    case A5: return std::pair{ADC_UNIT_2, ADC_CHANNEL_0};
    case 37: return std::pair{ADC_UNIT_1, ADC_CHANNEL_1};
    case 35: return std::pair{ADC_UNIT_1, ADC_CHANNEL_7};
    case 13: return std::pair{ADC_UNIT_2, ADC_CHANNEL_4};
    case 12: return std::pair{ADC_UNIT_2, ADC_CHANNEL_5};
    case 27: return std::pair{ADC_UNIT_2, ADC_CHANNEL_7};
    case 33: return std::pair{ADC_UNIT_1, ADC_CHANNEL_5};
    case 15: return std::pair{ADC_UNIT_2, ADC_CHANNEL_3};
    case 32: return std::pair{ADC_UNIT_1, ADC_CHANNEL_4};
    case 14: return std::pair{ADC_UNIT_2, ADC_CHANNEL_6};
    default: return std::nullopt;
    }
}

} // namespace pin_layout
} // namespace dev
