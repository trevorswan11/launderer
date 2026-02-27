#include <cmath>
#include <format>
#include <thread>

#include "lcd.hpp"
#include "potentiometer.hpp"
#include "rgb_led.hpp"
#include "touch.hpp"

using namespace idf;
using namespace std::chrono_literals;

const uint32_t DEEP_BLUE   = 0x1A2FF9;
const uint32_t HAPPY_COLOR = 0x28E96B;

extern "C" auto app_main() -> void {
    dev::Potentiometer pot{dev::pin_layout::A4};
    dev::Touch         touch{dev::pin_layout::A3};
    dev::RGBLed        rgb{dev::pin_layout::A7, dev::pin_layout::A8, dev::pin_layout::A9};
    dev::Lcd           lcd{0x27};

    lcd.init();

    while (true) {
        lcd.clear();
        const auto pot_percent = static_cast<float>(pot.read()) / pot.max_value();
        const auto touched     = touch.is_pressed();
        const auto base_color  = touched ? HAPPY_COLOR : DEEP_BLUE;
        rgb.update(base_color, pot_percent);

        lcd.print(touched ? "YAY! Thank you!" : "I'm so blue :(", 0, 0);
        const auto led_str = std::format("LED = {}", rgb.to_string());
        lcd.print(led_str, 0, 1);

        std::this_thread::sleep_for(50ms);
    }
}
