#pragma once

#include <string>

#include "pins.hpp"

namespace dev {

struct RGBChannel {
    PwmOut  pin;
    uint8_t value = 0;
};

class RGBLed {
  public:
    explicit RGBLed(Pin red, Pin green, Pin blue);

    auto set_red(uint8_t value) -> void;
    auto set_green(uint8_t value) -> void;
    auto set_blue(uint8_t value) -> void;

    auto update(uint8_t r, uint8_t g, uint8_t b) -> void;
    auto update(uint32_t hex) -> void;
    auto update(uint32_t hex, float scale) -> void;

    auto to_string() const -> std::string;

  private:
    RGBChannel red_;
    RGBChannel green_;
    RGBChannel blue_;
};

} // namespace dev
