#include <cmath>
#include <cstdint>
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

using u8    = std::uint8_t;
using i8    = std::int8_t;
using u16   = std::uint16_t;
using i16   = std::int16_t;
using u32   = std::uint32_t;
using i32   = std::int32_t;
using u64   = std::uint64_t;
using i64   = std::int64_t;
using usize = std::size_t;

static constexpr usize HISTORY_SIZE{20};

dev::Lcd lcd{0x27};

dev::DigitalPinIn white_tare{15, idf::GPIOPullMode::PULLDOWN()};
dev::DigitalPinIn red{33, idf::GPIOPullMode::PULLDOWN()};
dev::DigitalPinIn blue{14, idf::GPIOPullMode::PULLDOWN()};
dev::DigitalPinIn yellow{32, idf::GPIOPullMode::PULLDOWN()};
dev::DigitalPinIn white_reset{27, idf::GPIOPullMode::PULLDOWN()};

dev::PwmOut   pump{dev::pin_layout::A0};
dev::AnalogIn pressure_sensor{dev::pin_layout::A1};

// From desmos fit
constexpr float FIT_A        = -478286.117341f;
constexpr float FIT_B        = 830.395033677f;
constexpr float FIT_C        = 580.401879334f;
constexpr float STONE_WEIGHT = 2.5f;

[[nodiscard]] auto weight_from_adc(i32 zeroed) noexcept -> float {
    if (zeroed <= 0) { return 0.0f; }
    const auto stones = FIT_A / (zeroed - FIT_C) - FIT_B;
    return stones * STONE_WEIGHT;
}

extern "C" auto app_main() -> void {
    lcd.init();

    std::array<i32, HISTORY_SIZE> history{};
    usize                         history_index = 0;
    bool                          history_full  = false;

    i32  tare_offset = 0;
    bool tared       = false;

    const auto current_average = [&]() -> i32 {
        usize count = history_full ? HISTORY_SIZE : history_index;
        if (count == 0) { return 0; }
        i32 sum = 0;
        for (usize i = 0; i < count; ++i) { sum += history[i]; }
        return sum / static_cast<i32>(count);
    };

    while (true) {
        // Every iteration should track a new reading
        auto raw               = pressure_sensor.read();
        history[history_index] = raw;
        history_index          = (history_index + 1) % HISTORY_SIZE;
        if (history_index == 0) { history_full = true; }

        i32 zeroed = current_average() - tare_offset;

        lcd.clear();
        pump.write(4'095);

        if (white_reset) {
            tared         = false;
            tare_offset   = 0;
            history_index = 0;
            history_full  = false;
            history.fill(0);
        }

        if (tared) {
            lcd.print("Tared Scale :)");
            lcd.set_cursor(0, 1);
            lcd.print("Select Dirt");
        } else {
            lcd.print("Tare scale!");
        }
        std::println("raw={} avg={} zeroed={}", raw, current_average(), zeroed);

        std::optional<Dirtiness> dirt;
        if (white_tare) {
            tare_offset = current_average();
            tared       = true;
        } else if (tared) {
            const auto weight = weight_from_adc(zeroed);
            std::println("weight={}", weight);

            if (red) {
                dirt.emplace(Dirtiness::NORMAL);
            } else if (blue) {
                dirt.emplace(Dirtiness::DIRTY);
            } else if (yellow) {
                dirt.emplace(Dirtiness::NASTY);
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
