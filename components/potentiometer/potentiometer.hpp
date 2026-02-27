#pragma once

#include "pins.hpp"

namespace dev {

class Potentiometer {
  public:
    explicit Potentiometer(Pin pin) : pin_{pin} {}

    [[nodiscard]] auto read() -> int { return pin_.read(); }
    [[nodiscard]] auto max_value() -> int { return pin_.get_range(); }

  private:
    AnalogIn pin_;
};

} // namespace dev
