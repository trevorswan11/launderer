#pragma once

#include "pins.hpp"

namespace dev {

class Touch {
  public:
    explicit Touch(Pin pin) : pin_{pin} {}

    [[nodiscard]] auto is_pressed() -> bool { return pin_.read() > (pin_.get_range() / 2); }

  private:
    AnalogIn pin_;
};

} // namespace dev
