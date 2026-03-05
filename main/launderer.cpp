#include <cmath>
#include <optional>
#include <print>
#include <thread>

#include "gpio_cxx.hpp"
#include "lcd.hpp"
#include "pin_layout.hpp"
#include "pins.hpp"

using namespace std::chrono_literals;

enum class Dirtiness {
    NORMAL,
    DIRTY,
    NASTY,
};

extern "C" auto app_main() -> void {
    dev::Lcd lcd{0x27};
    lcd.init();

    dev::DigitalPinIn white{15};
    dev::DigitalPinIn red{33};
    dev::DigitalPinIn blue{14};
    dev::DigitalPinIn yellow{32};

    dev::PwmOut pump{dev::pin_layout::A0};
    bool        tared = false;

    while (true) {
        lcd.clear();
        pump.write(4'095);
        const bool white_pressed = !white;
        if (tared) {
            lcd.print("Tared Scale :)");
            lcd.set_cursor(0, 1);
            lcd.print("Select Dirt");
        } else {
            lcd.print("Tare scale!");
        }

        std::optional<Dirtiness> dirt;
        if (white_pressed) {
            tared = true;
        } else if (tared) {
            if (!red) {
                dirt = Dirtiness::NORMAL;
            } else if (!blue) {
                dirt = Dirtiness::DIRTY;
            } else if (!yellow) {
                dirt = Dirtiness::NASTY;
            }

            if (dirt) {
                lcd.clear();
                tared = false;

                switch (*dirt) {
                case Dirtiness::NORMAL: lcd.print("Normal load!"); break;
                case Dirtiness::DIRTY:  lcd.print("Dirty load!"); break;
                case Dirtiness::NASTY:  lcd.print("Nasty load!"); break;
                }

                lcd.set_cursor(0, 1);
                lcd.print("Dispensing!");

                pump.write(0);
                std::this_thread::sleep_for(2s);
                pump.write(4'095);
            }
        }

        std::this_thread::sleep_for(50ms);
    }
}
