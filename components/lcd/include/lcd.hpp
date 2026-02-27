#pragma once

#include <memory>
#include <string>

#include "pin_layout.hpp"

struct hd44780;

namespace dev {

class Lcd {
  public:
    explicit Lcd(uint8_t addr, Pin sda = pin_layout::SDA, Pin scl = pin_layout::SCL);
    ~Lcd();

    auto init() -> void;
    auto set_cursor(uint8_t col, uint8_t row) -> void;
    auto print(const std::string& text) -> void;
    auto print(const std::string& text, uint8_t col, uint8_t row) -> void;
    auto clear() -> void;

  private:
    std::unique_ptr<hd44780> lcd_config_;
    uint8_t                  addr_;
    Pin                      sda_;
    Pin                      scl_;
};

} // namespace dev
